#include "nb_arm/controllers/arm_control_sim.hpp"
#include <kdl/path_line.hpp>
#include <kdl/rotational_interpolation_sa.hpp>

ArmControlSim::ArmControlSim(rclcpp::Node::SharedPtr node)
    : node_(node), is_running_(true), controller_(node)
{
    RCLCPP_INFO(node_->get_logger(), "ArmControlSim has been initialized.");

    // Initialize joint names in the desired order
    joint_names_ = {
        "shoulder_pan_joint",
        "shoulder_lift_joint",
        "elbow_joint",
        "wrist_1_joint",
        "wrist_2_joint",
        "wrist_3_joint"};

    robot_cmd_pub_ = node_->create_publisher<std_msgs::msg::Float64MultiArray>("/joint_group_effort_controller/commands", 10);
    robot_state_sub_ = node_->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10, std::bind(&ArmControlSim::robot_callback, this, std::placeholders::_1));
    sim_thread_ = std::thread(&ArmControlSim::run_control, this);
}

// 析构函数：停止线程
ArmControlSim::~ArmControlSim()
{
    is_running_ = false;
    if (sim_thread_.joinable())
    {
        sim_thread_.join(); // 等待线程结束
    }
    RCLCPP_INFO(node_->get_logger(), "ArmControlSim has been shut down.");
}

void ArmControlSim::send_robot_command(std_msgs::msg::Float64MultiArray cmd_msg, const float tor_ff[MOTOR_NUM])
{
    if (cmd_msg.data.size() != MOTOR_NUM)
    {
        RCLCPP_WARN(node_->get_logger(), "send_robot_command: Input command size is incorrect.");
        std::cerr << "Expected size: " << MOTOR_NUM << ", but got: " << cmd_msg.data.size() << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(cmd_mutex_);

        cmd_msg.data.resize(MOTOR_NUM);
        for (size_t i = 0; i < MOTOR_NUM; ++i)
        {
            float desired_torque = tor_ff[i] +
                                   motor_sim.mit_kp[i] * (motor_sim.desire_motor_pos[i] - motor_sim.current_motor_pos[i]) +
                                   motor_sim.mit_kd[i] * (motor_sim.desire_motor_vel[i] - motor_sim.current_motor_vel[i]);

            // motor.desire_motor_tor[i] = std::clamp(motor.desire_motor_tor[i], motor.min_motor_tor[i], motor.max_motor_tor[i]);
            cmd_msg.data[i] = desired_torque;
        }
        robot_cmd_pub_->publish(cmd_msg);
    }
}

void ArmControlSim::robot_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    {
        for (size_t i = 0; i < msg->name.size(); ++i)
        {
            auto it = std::find(joint_names_.begin(), joint_names_.end(), msg->name[i]);
            if (it != joint_names_.end())
            {
                size_t joint_index = std::distance(joint_names_.begin(), it);
                if (joint_index < ARM_DOF)
                {
                    motor_sim.current_motor_pos[joint_index] = msg->position[i];
                    if (i < msg->velocity.size())
                    {
                        motor_sim.current_motor_vel[joint_index] = msg->velocity[i];
                        // std::cout << "Received joint vel: " << motor_sim.current_motor_vel[joint_index] << std::endl;
                    }
                    if (i < msg->effort.size())
                    {
                        motor_sim.current_motor_tor[joint_index] = msg->effort[i];
                        // std::cout << "Received joint tor: " << motor_sim.current_motor_tor[joint_index] << std::endl;
                    }
                    // std::cout << "Received joint pos: " << motor_sim.current_motor_pos[joint_index] << std::endl;
                }
            }
        }
    }
}

void ArmControlSim::run_control()
{
    const std::vector<KDL::Frame> test_waypoints = {
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.2, 0)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.3, 0.5, 0.1)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.25, 0.5)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.2, 0.2, 0.2)),
    };
    controller_.path_ = controller_.traj_generator_.generate_rounded_path(test_waypoints);
    // controller_.path_ = controller_.traj_generator_.generate_line_path(test_waypoints);
    // bool trajectory_valid = controller_.get_trajectory(controller_.arm_model_, controller_.traj_gen_, std::move(controller_.path_), false, 5.0);
    bool trajectory_valid = controller_.get_trajectory(controller_.arm_model_, controller_.traj_gen_, std::move(controller_.path_), true, 5.0);

    if (!trajectory_valid || controller_.q_ref_trajectory_.empty())
    {
        std::cerr << "错误: 控制循环开始前轨迹未生成或为空！" << std::endl;
        return;
    }
    controller_.arm_model_.update_armState(
        motor_sim.current_motor_pos,
        motor_sim.current_motor_vel,
        motor_sim.current_motor_tor);
    // controller_.calibrate_external_force(controller_.arm_model_, controller_.arm_model_.current_joint_torques);
    const auto loop_period = std::chrono::milliseconds(static_cast<long>(1000.0 / control_frequency));
    // float dt = 1.0 / control_frequency;
    size_t trajectory_size = controller_.q_ref_trajectory_.size();
    std::cout << "开始执行控制循环..." << std::endl;

    // 重置起始搜索索引
    last_closest_index_ = 0;
    const int lookahead_points = 15;        // 前瞻点
    size_t target_index = lookahead_points; // 在循环外初始化目标索引

    auto next_cycle_time = std::chrono::steady_clock::now();
    // 使用 while 循环代替 for 循环，直到到达轨迹终点
    while (last_closest_index_ < trajectory_size - 1)
    {
        std_msgs::msg::Float64MultiArray effort_cmd_msg;
        effort_cmd_msg.data.resize(MOTOR_NUM);
        // if ((last_closest_index_ >= trajectory_size - 10) && !controller_.enable_touch_detection)
        // {
        //     std::cout << "即将靠近接触点" << std::endl;
        //     // 启用接触检测
        //     controller_.set_touch_detection(true, 10);
        // }
        controller_.arm_model_.update_armState(
            motor_sim.current_motor_pos,
            motor_sim.current_motor_vel,
            motor_sim.current_motor_tor);
        controller_.arm_model_.jac_solver->JntToJac(controller_.arm_model_.current_joint_positions, controller_.arm_model_.current_jacobian);

        size_t closest_index;
        if (!controller_.touch_detected)
        {
            float min_dist_sq = std::numeric_limits<float>::max();
            closest_index = last_closest_index_;
            for (size_t i = last_closest_index_; i < trajectory_size; ++i)
            {
                float dist_sq = controller_.distanceSq(controller_.arm_model_.current_joint_positions, controller_.q_ref_trajectory_[i]);
                if (dist_sq < min_dist_sq)
                {
                    min_dist_sq = dist_sq;
                    closest_index = i;
                }
            }
            last_closest_index_ = closest_index; // 更新进度

            // 检查是否到达当前目标点
            float distance_to_target = controller_.distanceSq(controller_.arm_model_.current_joint_positions, controller_.q_ref_trajectory_[target_index]);
            const float arrival_threshold = 0.01f; // 到达阈值，需要根据实际情况调整
            if (distance_to_target < arrival_threshold)
            {
                // 到达后，更新到下一个目标点
                target_index = std::min(closest_index + lookahead_points, trajectory_size - 1);
            }
        }
        else
        {
            last_closest_index_++;
            closest_index = last_closest_index_;
        }

        if (target_index >= trajectory_size)
        {
            target_index = trajectory_size - 1;
        }
        const KDL::JntArray &q_ref = controller_.q_ref_trajectory_[target_index];
        const KDL::JntArray &qdot_ref = controller_.qdot_ref_trajectory_[target_index];
        const KDL::JntArray &qddot_ref = controller_.qddot_ref_trajectory_[target_index];
        // controller_.admittance_control_update(controller_.arm_model_, q_ref, qdot_ref, qddot_ref, dt);
        // for (size_t i = 0; i < MOTOR_NUM; ++i)
        // {
        //     motor_sim.desir_motor_pos[i] = q_ref(i);
        //     motor_sim.desir_motor_vel[i] = qdot_ref(i);
        // }

        // if (controller_.enable_touch_detection && !controller_.touch_detected)
        // {
        //     controller_.estimate_eef_wrench_frame(controller_.arm_model_);
        //     // 假设吸盘沿工具坐标系的Z轴方向接近物体
        //     // f_ext_eigen[2] 就是沿基坐标系Z轴的力
        //     float force_z = controller_.f_ext_eef.force.z();
        //     if (std::abs(force_z) > controller_.touch_force_threshold_z_) // 阈值检测判断
        //     {
        //         std::cout << "检测到接触!当前Z轴接触力: " << force_z << " N" << std::endl;
        //         controller_.touch_detected = true;
        //     }
        // }
        KDL::JntArray tor_ff(ARM_DOF);
        controller_.arm_model_.computeInverseDynamics(q_ref, qdot_ref, qddot_ref, tor_ff);

        controller_.arm_model_.desire_joint_positions = q_ref;
        controller_.arm_model_.desire_joint_velocities = qdot_ref;
        controller_.arm_model_.desire_joint_acceleration = qddot_ref;
        
        // Convert KDL::JntArray to float array for joint2motor
        float q_ref_arr[ARM_DOF];
        float qdot_ref_arr[ARM_DOF];
        float qddot_ref_arr[ARM_DOF];
        float tor_ff_arr[ARM_DOF];
        
        for(int i=0; i<ARM_DOF; ++i) {
            q_ref_arr[i] = controller_.arm_model_.desire_joint_positions(i);
            qdot_ref_arr[i] = controller_.arm_model_.desire_joint_velocities(i);
            qddot_ref_arr[i] = controller_.arm_model_.desire_joint_acceleration(i);
            tor_ff_arr[i] = tor_ff(i);
        }

        controller_.arm_model_.joint2motor(
            q_ref_arr,
            qdot_ref_arr,
            qddot_ref_arr,
            tor_ff_arr,
            motor_sim.desire_motor_pos,
            motor_sim.desire_motor_vel,
            motor_sim.desire_motor_acc,
            motor_sim.desire_motor_tor);
        send_robot_command(effort_cmd_msg, motor_sim.desire_motor_tor);
        next_cycle_time += loop_period;
        std::this_thread::sleep_until(next_cycle_time);

        // 检查是否到达轨迹终点附近
        if (closest_index >= trajectory_size - 1)
        {
            std::cout << "已到达轨迹终点。" << std::endl;
            break;
        }
    }
    // --- 修改: 循环结束后的状态检查 ---
    if (controller_.enable_touch_detection)
    {
        if (controller_.touch_detected)
        {
            std::cout << "任务成功: 轨迹结束, 且已保持接触。" << std::endl;
        }
        else
        {
            std::cout << "任务失败: 轨迹结束, 但未检测到接触。" << std::endl;
        }
    }
    else
    {
        std::cout << "轨迹执行完毕 (未启用接触检测)。" << std::endl;
    }
}