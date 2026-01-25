#include "nb_arm/controllers/arm_control.hpp"
#include <iostream>
#include <vector>
#include <memory>

arm_control::arm_control(rclcpp::Node::SharedPtr node) : node_(node)
{
    // 初始化导纳控制参数
    admittance_pos_offset_jnt_.resize(ARM_DOF);
    admittance_vel_offset_jnt_.resize(ARM_DOF);
    admittance_pos_offset_jnt_temp.resize(ARM_DOF);
    admittance_vel_offset_jnt_temp.resize(ARM_DOF);
    tor_ext_filtered_.resize(ARM_DOF);
    tor_ext_bias.resize(ARM_DOF);
    f_ext_eef.force.x(0.0);
    f_ext_eef.force.y(0.0);
    f_ext_eef.force.z(0.0);
    f_ext_eef.torque.x(0.0);
    f_ext_eef.torque.y(0.0);
    f_ext_eef.torque.z(0.0);

    // 使用 TransientLocal QoS 策略，确保后加入的订阅者（如 RViz）也能收到最后发布的消息
    rclcpp::QoS qos_profile(10);
    qos_profile.transient_local();
    trajectory_pub_ = node_->create_publisher<nav_msgs::msg::Path>("/arm_trajectory_viz", qos_profile);

    // 初始化所有关节的偏移量和滤波力矩为零
    for (unsigned int i = 0; i < ARM_DOF; ++i)
    {
        admittance_pos_offset_jnt_(i) = 0.0;
        admittance_vel_offset_jnt_(i) = 0.0;
        tor_ext_filtered_(i) = 0.0;
        tor_ext_bias(i) = 0.0;
    }
}

float arm_control::distanceSq(const KDL::JntArray &a, const KDL::JntArray &b)
{
    float dist = 0.0f;
    for (unsigned int i = 0; i < a.rows(); ++i)
    {
        float diff = a(i) - b(i);
        dist += diff * diff;
    }
    return dist;
}

/**
 * @brief 生成笛卡尔轨迹，并预计算整条关节空间轨迹
 * @return bool 是否成功生成轨迹
 */
bool arm_control::get_trajectory(
    nb_arm_xdof &arm_model,
    trajectory_generator &traj_gen,
    std::unique_ptr<KDL::Path> path,
    bool is_trap,
    float spline_duration)
{
    // 生成笛卡尔空间轨迹
    trajectory_com = std::make_unique<KDL::Trajectory_Composite>();
    auto trajectory_segment = traj_gen.generate_trajectory_seg(std::move(path), is_trap, spline_duration);
    // ...更多轨迹可以继续生成并添加到 trajectory_com 中

    if (!trajectory_segment)
    {
        std::cerr << "错误: 生成轨迹段失败！" << std::endl;
        return false;
    }
    trajectory_com->Add(trajectory_segment.release());

    // 预计算关节空间轨迹
    std::cout << "开始预计算关节空间轨迹..." << std::endl;
    q_ref_trajectory_.clear();
    qdot_ref_trajectory_.clear();
    qddot_ref_trajectory_.clear();

    float dt = 1.0f / control_frequency;
    float total_duration = static_cast<float>(trajectory_com->Duration());
    int num_points = static_cast<int>(total_duration / dt);

    // 使用机械臂当前位置作为IK的初始种子点
    KDL::JntArray q_seed = arm_model.current_joint_positions;
    KDL::JntArray q_out(ARM_DOF);
    KDL::JntArray qdot_out(ARM_DOF);

    // 遍历整条轨迹的所有离散时间点
    for (int i = 0; i <= num_points; ++i)
    {
        // 1. 获取当前时间点的笛卡尔目标
        float t = i * dt;                             // 计算当前时间点
        KDL::Frame x_ref = trajectory_com->Pos(t);     // 获取此时末端执行器应在的【位置和姿态】
        KDL::Twist xdot_ref = trajectory_com->Vel(t);  // 获取此时末端执行器应有的【线速度和角速度】
        // 2. 求解逆向运动学
        // 求解逆位置：根据笛卡尔位置(x_ref)，计算出对应的关节角度(q_out)
        int ik_pos_ret = arm_model.ik_solver_pos_lma->CartToJnt(q_seed, x_ref, q_out);
        // 求解逆速度：根据笛卡尔速度(xdot_ref)和当前关节角度(q_out)，计算出对应的关节速度(qdot_out)
        int ik_vel_ret = arm_model.ik_solver_vel_wdls->CartToJnt(q_out, xdot_ref, qdot_out);

        // 3. 处理求解失败的情况
        if (ik_pos_ret < 0 || ik_vel_ret < 0)
        {
            // 如果IK求解失败（比如目标点超出了机械臂工作范围，或接近奇异点）
            std::cerr << "警告: 轨迹预计算IK失败,时间点: " << t << "秒。将使用上一个有效点。" << std::endl;
            if (!q_ref_trajectory_.empty())
            {
                // 如果不是第一个点就失败，就用上一个成功计算出的点作为当前点
                q_out = q_ref_trajectory_.back();
                SetToZero(qdot_out); // 同时把速度设为0，防止突变，保证安全
            }
            else
            {
                // 如果连第一个点都计算失败，那整条轨迹都无法生成，直接报错返回
                std::cerr << "错误: 轨迹起点IK失败,无法继续!" << std::endl;
                return false;
            }
        }

        // 4. 存储计算结果并为下一次计算做准备
        q_ref_trajectory_.push_back(q_out);       // 将计算出的关节位置存入轨迹列表
        qdot_ref_trajectory_.push_back(qdot_out); // 将计算出的关节速度存入轨迹列表

        // 【关键优化】
        // 使用本次计算出的结果 q_out 作为下一次计算的 "种子点" (q_seed)
        // 因为轨迹是连续的，下一个时间点的解很可能就在当前解的附近。
        q_seed = q_out;
    }
    // resize 加速度向量
    size_t trajectory_size = q_ref_trajectory_.size();
    qddot_ref_trajectory_.resize(trajectory_size);

    // 显式调整每个 JntArray 的大小
    for (size_t i = 0; i < trajectory_size; ++i)
    {
        qddot_ref_trajectory_[i].resize(ARM_DOF);
    }

    // 设置第一个点的加速度为0
    SetToZero(qddot_ref_trajectory_[0]);
    // 离散速度点计算加速度
    for (size_t i = 1; i < trajectory_size; i++)
    {
        // qddot(i) = (qdot(i) - qdot(i-1)) / dt
        KDL::Subtract(qdot_ref_trajectory_[i], qdot_ref_trajectory_[i - 1], qddot_ref_trajectory_[i]);
        KDL::Divide(qddot_ref_trajectory_[i], dt, qddot_ref_trajectory_[i]);
    }

    std::cout << "关节空间轨迹预计算完毕,共 " << q_ref_trajectory_.size() << " 个点。" << std::endl;
    
    publish_trajectory_visualization(arm_model);

    return true;
}
/**
 * @brief 单步导纳控制更新
 */
void arm_control::admittance_control_update(
    nb_arm_xdof &arm_model,
    const KDL::JntArray &q_ref,
    const KDL::JntArray &qdot_ref,
    const KDL::JntArray &qddot_ref,
    float dt)
{
    // 力矩估计 (计算 τ_ext)
    KDL::JntArray tor_model(ARM_DOF);
    // KDL::JntArray zero_acc(ARM_DOF);
    // SetToZero(zero_acc);

    // 使用当前【实际】的关节状态来计算模型力矩,前提是使用通信接口实时更新关节状态
    arm_model.computeInverseDynamics(
        arm_model.current_joint_positions,
        arm_model.current_joint_velocities,
        qddot_ref,
        tor_model);

    KDL::JntArray tor_ext = arm_model.current_joint_torques;
    KDL::Subtract(tor_ext, tor_model, tor_ext); // 电机力矩减去模型力矩得到外部力矩

    /*
    // --- 新增: 摩擦力补偿 ---
    // 模型: Tau_f = F_c * sgn(vel) + F_v * vel
    // 注意: 这些系数需要根据您的机械臂实际情况进行辨识!
    // 这里仅作为示例值 (假设前3轴摩擦大, 后3轴小)
    const float fric_coulomb[6] = {0.5f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f}; // 库伦摩擦系数 (Nm)
    const float fric_viscous[6] = {1.5f, 1.5f, 1.5f, 0.5f, 0.5f, 0.5f}; // 粘滞摩擦系数 (Nm / (rad/s))

    KDL::JntArray tor_friction(ARM_DOF);
    for(size_t i=0; i<ARM_DOF; ++i) {
        float vel = arm_model.current_joint_velocities(i);
        float sign_vel = 0.0f;

        // 使用死区防止零速附近的震荡 (chattering)
        const float vel_deadband = 0.02f;
        if (vel > vel_deadband) sign_vel = 1.0f;
        else if (vel < -vel_deadband) sign_vel = -1.0f;
        else sign_vel = vel / vel_deadband; // 在死区内线性过渡

        tor_friction(i) = fric_coulomb[i] * sign_vel + fric_viscous[i] * vel;
    }

    // 从外部力矩中减去摩擦力矩 (因为摩擦力也是一种"内部"消耗)
    KDL::Subtract(tor_ext, tor_friction, tor_ext);
    // -----------------------
    */

    KDL::Subtract(tor_ext, tor_ext_bias, tor_ext); // 再减去零点偏置

    // 以上方法计算的外部力矩噪声较大，对外部力矩进行低通滤波
    const float filter_alpha = 0.7; // 滤波系数，越接近1越平滑，但延迟越大
    for (size_t i = 0; i < ARM_DOF; ++i)
    {
        tor_ext_filtered_(i) = filter_alpha * tor_ext_filtered_(i) + (1.0 - filter_alpha) * tor_ext(i);
    }

    // 关节空间导纳动力学
    // M*q_ddot_adm + D*q_dot_adm + K*q_adm = τ_ext_filtered
    const float M_jnt_diag[6] = {1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f};
    const float D_jnt_diag[6] = {50.0f, 50.0f, 50.0f, 20.0f, 20.0f, 20.0f};
    const float K_jnt_diag[6] = {100.0f, 100.0f, 100.0f, 40.0f, 40.0f, 40.0f};

    KDL::JntArray acc_adm_jnt(ARM_DOF); // q_ddot_adm
    for (size_t i = 0; i < ARM_DOF; ++i)
    {
        float spring_force = K_jnt_diag[i] * admittance_pos_offset_jnt_(i);                   // 弹性力
        float damper_force = D_jnt_diag[i] * admittance_vel_offset_jnt_(i);                   // 阻尼力
        acc_adm_jnt(i) = (tor_ext_filtered_(i) - damper_force - spring_force) / M_jnt_diag[i]; // q_ddot_adm
    }

    // 欧拉积分 (更新导纳状态)
    // 更新速度偏移
    KDL::Multiply(acc_adm_jnt, dt, admittance_vel_offset_jnt_temp);
    KDL::Add(admittance_vel_offset_jnt_, admittance_vel_offset_jnt_temp, admittance_vel_offset_jnt_);
    // 更新位置偏移
    KDL::Multiply(admittance_vel_offset_jnt_, dt, admittance_pos_offset_jnt_temp);
    KDL::Add(admittance_pos_offset_jnt_, admittance_pos_offset_jnt_temp, admittance_pos_offset_jnt_);

    // 为导纳偏移量增加饱和限制，防止积分饱和
    const double max_pos_offset = 0.35f; // rad, 约20度
    const double max_vel_offset = 1.0f;  // rad/s
    for (size_t i = 0; i < ARM_DOF; ++i)
    {
        admittance_pos_offset_jnt_(i) = std::clamp(admittance_pos_offset_jnt_(i), -max_pos_offset, max_pos_offset);
        admittance_vel_offset_jnt_(i) = std::clamp(admittance_vel_offset_jnt_(i), -max_vel_offset, max_vel_offset);
    }

    // --- 4. 目标融合 (计算最终目标) ---
    // 最终位置目标 Q_target = Q_ref + q_adm
    KDL::Add(q_ref, admittance_pos_offset_jnt_, arm_model.desire_joint_positions);
    // 最终速度目标 V_target = V_ref + q_dot_adm
    KDL::Add(qdot_ref, admittance_vel_offset_jnt_, arm_model.desire_joint_velocities);
    arm_model.desire_joint_acceleration = qddot_ref;
}

/**
 * @brief 在开始运动前调用，用于标定外部力矩估计的零点偏置
 */
void arm_control::calibrate_external_force(nb_arm_xdof &arm_model, const KDL::JntArray &current_motor_torques)
{
    std::cout << "正在标定外部力矩零点偏置..." << std::endl;
    KDL::JntArray tor_model(ARM_DOF);
    // 假设机械臂静止，速度和加速度为0
    KDL::JntArray zero_vel(ARM_DOF), zero_acc(ARM_DOF);
    SetToZero(zero_vel);
    SetToZero(zero_acc);
    arm_model.computeInverseDynamics(
        arm_model.current_joint_positions,
        zero_vel,
        zero_acc,
        tor_model);

    KDL::Subtract(current_motor_torques, tor_model, tor_ext_bias); // 计算偏置
    tor_ext_filtered_ = tor_ext_bias;                              // 初始化滤波器
    std::cout << "零点偏置标定完成。" << std::endl;
}

/**
 * @brief 估计末端力，并将结果存储在 f_ext_eef_ (工具坐标系)
 * @param arm_model 机械臂模型
 */
void arm_control::estimate_eef_wrench_frame(nb_arm_xdof &arm_model)
{
    if (arm_model.jac_solver->JntToJac(arm_model.current_joint_positions, arm_model.current_jacobian) < 0)
    {
        std::cerr << "错误: 估计末端力时更新雅各比失败！" << std::endl;
        return;
    }
    // 计算 J^T 的伪逆
    Eigen::MatrixXd Jt = arm_model.current_jacobian.data.transpose(); // 求雅各比矩阵的转置

    Eigen::MatrixXd Jt_pinv = Jt.completeOrthogonalDecomposition().pseudoInverse(); // 计算伪逆
    Eigen::VectorXd tor_ext_eigen(ARM_DOF);
    for (int i = 0; i < ARM_DOF; ++i)
        tor_ext_eigen(i) = tor_ext_filtered_(i);
    Eigen::VectorXd f_ext_eigen = Jt_pinv * tor_ext_eigen;

    if (!f_ext_eigen.allFinite())
    {
        std::cerr << "警告: 末端力估计求解失败 (可能接近奇异点)。" << std::endl;
        SetToZero(f_ext_eef);
        return;
    }
    // 获取当前的末端姿态
    KDL::Frame eef_frame;
    if (arm_model.fk_solver_pos->JntToCart(arm_model.current_joint_positions, eef_frame) < 0)
    {
        std::cerr << "错误: 估计末端力时计算正向运动学失败！" << std::endl;
        return;
    }
    // 将基坐标系的力 f_base 旋转到 工具坐标系 f_tool
    // f_tool = R_base_tool^T * f_base
    KDL::Vector f_base(f_ext_eigen[0], f_ext_eigen[1], f_ext_eigen[2]);
    KDL::Vector f_tool = eef_frame.M.Inverse() * f_base; // M.Inverse() == R^T

    // 转换力矩
    KDL::Vector t_base(f_ext_eigen[3], f_ext_eigen[4], f_ext_eigen[5]);
    KDL::Vector t_tool = eef_frame.M.Inverse() * t_base;

    // 存储结果
    f_ext_eef.force = f_tool;
    f_ext_eef.torque = t_tool;
}

void arm_control::set_touch_detection(bool enable, float threshold_z)
{
    enable_touch_detection = enable;
    touch_force_threshold_z_ = threshold_z;
    touch_detected = false; // 设置检测模式后重置检测状态
    SetToZero(f_ext_eef);
}

bool arm_control::is_touch_detected() const
{
    return touch_detected;
}

/**
 * @brief 主控制循环
 */

void arm_control::control_loop(nb_arm_xdof &arm_model, motor_driver &motor)
{
    if (q_ref_trajectory_.empty())
    {
        std::cerr << "错误: 控制循环开始前轨迹未生成或为空！" << std::endl;
        return;
    }

    arm_model.update_armState(motor.current_motor_pos,
                              motor.current_motor_vel,
                              motor.current_motor_tor);

    calibrate_external_force(arm_model, arm_model.current_joint_torques); // 力矩零点偏置标定

    const auto loop_period = std::chrono::milliseconds(static_cast<long>(1000.0 / control_frequency));
    float dt = 1.0f / control_frequency;
    size_t trajectory_size = q_ref_trajectory_.size();
    std::cout << "开始执行控制循环..." << std::endl;

    // 重置起始搜索索引
    last_closest_index_ = 0;

    auto next_cycle_time = std::chrono::steady_clock::now();

    // 使用 while 循环代替 for 循环，直到到达轨迹终点
    while (last_closest_index_ < trajectory_size - 1)
    {
        if ((last_closest_index_ >= trajectory_size - 10) && !enable_touch_detection)
        {
            std::cout << "即将靠近终点" << std::endl;
            // 启用接触检测
            set_touch_detection(true, 10);
        }
        // --- 1. 从驱动器更新机械臂实际状态 ---
        arm_model.update_armState(motor.current_motor_pos,
                                  motor.current_motor_vel,
                                  motor.current_motor_tor); // 这部分可以写在其他位置实时更新

        arm_model.jac_solver->JntToJac(arm_model.current_joint_positions, arm_model.current_jacobian);
        size_t target_index;
        size_t closest_index;
        if (!touch_detected)
        {
            // --- 2. 在参考轨迹上寻找新的目标点 ---
            // 2.1 找到离当前位置最近的点
            float min_dist_sq = std::numeric_limits<float>::max();
            closest_index = last_closest_index_;
            // 从上一次的位置开始搜索，避免每次都从头开始，提高效率
            for (size_t i = last_closest_index_; i < trajectory_size; ++i)
            {
                float dist_sq = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[i]);
                if (dist_sq < min_dist_sq)
                {
                    min_dist_sq = dist_sq;
                    closest_index = i;
                }
            }
            last_closest_index_ = closest_index; // 更新进度
            // 2.2 确定前瞻目标点
            const int lookahead_points = 15; // 前瞻点数，这是一个关键的可调参数
            target_index = std::min(closest_index + lookahead_points, trajectory_size - 1);
        }
        else
        {
            last_closest_index_++;
            closest_index = last_closest_index_;
        }
        // 确保 target_index 不会越界 (安全措施)
        if (target_index >= trajectory_size)
        {
            target_index = trajectory_size - 1;
        }

        // 2.3 获取目标点的
        const KDL::JntArray &q_ref = q_ref_trajectory_[target_index];
        const KDL::JntArray &qdot_ref = qdot_ref_trajectory_[target_index];
        const KDL::JntArray &qddot_ref = qddot_ref_trajectory_[target_index];

        // --- 3. 执行导纳控制更新 ---
        admittance_control_update(arm_model, q_ref, qdot_ref, qddot_ref, dt);
        // for (size_t i = 0; i < MOTOR_NUM; ++i)
        // {
        //     motor.desir_motor_pos[i] = q_ref(i);
        //     motor.desir_motor_vel[i] = qdot_ref(i);
        // }

        // --- 4. 检测触摸 ---
        if (enable_touch_detection && !touch_detected)
        {
            estimate_eef_wrench_frame(arm_model);
            // 假设吸盘沿工具坐标系的Z轴方向接近物体
            // f_ext_eigen[2] 就是沿基坐标系Z轴的力
            float force_z = f_ext_eef.force.z();
            if (std::abs(force_z) > touch_force_threshold_z_) // 阈值检测判断
            {
                std::cout << "检测到接触!当前Z轴接触力: " << force_z << " N" << std::endl;
                touch_detected = true;
            }
        }

        // --- 5. 计算前馈力矩 ---
        KDL::JntArray tor_ff(ARM_DOF);
        arm_model.computeInverseDynamics(q_ref, qdot_ref, qddot_ref, tor_ff);

        // --- 6. 发送电机命令 ---
        // ... (与之前相同)
        // arm_model.joint2motor(
        //     arm_model.desire_joint_positions.data,
        //     arm_model.desire_joint_velocities.data,
        //     arm_model.desire_joint_acceleration.data,
        //     tor_ff.data,
        //     motor.desir_motor_pos,
        //     motor.desir_motor_vel,
        //     motor.desir_motor_acc,
        //     motor.motor_control_tau);
        // motor.ControlMotors(
        //     motor.desir_motor_pos,
        //     motor.desir_motor_vel,
        //     motor.kp,
        //     motor.kd,
        //     motor.motor_control_tau);

        // --- 7. 维持固定的控制频率 ---
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
    if (enable_touch_detection)
    {
        if (touch_detected)
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

    // 在循环结束后发送一个保持当前位置的指令
}

void arm_control::publish_trajectory_visualization(nb_arm_xdof &arm_model)
{
    if (q_ref_trajectory_.empty())
        return;

    nav_msgs::msg::Path path_msg;
    path_msg.header.stamp = node_->now();
    path_msg.header.frame_id = "base_link"; // 请确保这与您的URDF中的基座坐标系名称一致

    for (const auto &q : qdot_ref_trajectory_)
    {
        KDL::Frame cart_pose;
        // 使用正运动学求解器计算笛卡尔位置
        if (arm_model.fk_solver_pos->JntToCart(q, cart_pose) >= 0)
        {
            geometry_msgs::msg::PoseStamped pose_stamped;
            pose_stamped.header = path_msg.header;
            
            pose_stamped.pose.position.x = cart_pose.p.x();
            pose_stamped.pose.position.y = cart_pose.p.y();
            pose_stamped.pose.position.z = cart_pose.p.z();

            double x, y, z, w;
            cart_pose.M.GetQuaternion(x, y, z, w);
            pose_stamped.pose.orientation.x = x;
            pose_stamped.pose.orientation.y = y;
            pose_stamped.pose.orientation.z = z;
            pose_stamped.pose.orientation.w = w;

            path_msg.poses.push_back(pose_stamped);
        }
    }

    trajectory_pub_->publish(path_msg);
    std::cout << "已发布轨迹可视化消息，包含 " << path_msg.poses.size() << " 个点。" << std::endl;
}