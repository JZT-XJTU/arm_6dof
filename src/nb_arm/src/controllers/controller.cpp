#include "nb_arm/controllers/controller.hpp"

controller::controller()
{
    control_frequency = 100.0; // 暂定控制频率为100Hz
    initialized = false;       // 初始化标志位
}

float controller::distanceSq(const KDL::JntArray &a, const KDL::JntArray &b)
{
    float dist = 0.0f;
    for (unsigned int i = 0; i < a.rows(); ++i)
    {
        float diff = a(i) - b(i);
        dist += diff * diff;
    }
    return dist;
}

void controller::modeSwitch(State newState)
{
    switch (newState)
    {
    case State::HybridForce:
        std::cout << "Transitioning to HybridForce state.\n";
        break;
    case State::Position:
        std::cout << "Transitioning to Position state.\n";
        break;
    case State::Zero:
        std::cout << "Transitioning to Zero state.\n";
        break;
    default:
        std::cerr << "Unknown state transition!" << std::endl;
        break;
    }

    // 更新当前状态
    currentState = newState;
}

void controller::run(nb_arm_xdof &arm_model, damiao::Motor_Control &motor)
{
    if (!initialized)
    {
        std::cout << "Initializing Robot Arm.\n";
        // 工作空间位型初始化
        go_home(arm_model, motor);
        initialized = true;
    }
    switch (currentState)
    {
    case State::HybridForce:
        HybridForceControl(arm_model, motor);
        break;
    case State::Position:
        PositionControl(arm_model, motor);
        break;
    case State::Zero:
        // SetZero(arm_model, motor);
        break;
    default:
        std::cerr << "Unknown state!" << std::endl;
        break;
    }
}

bool controller::get_trajectory(
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
    x_ref_trajectory_.clear();
    xdot_ref_trajectory_.clear();

    float dt = 1.0f / control_frequency;
    float total_duration = trajectory_com->Duration();
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
        KDL::Frame x_ref = trajectory_com->Pos(t);    // 获取此时末端执行器应在的【位置和姿态】
        KDL::Twist xdot_ref = trajectory_com->Vel(t); // 获取此时末端执行器应有的【线速度和角速度】
        // 2. 求解逆向运动学
        // 求解逆位置：根据笛卡尔位置(x_ref)，计算出对应的关节角度(q_out)
        int ik_pos_ret = arm_model.ik_solver_pos_lma->CartToJnt(q_seed, x_ref, q_out);
        // 求解逆速度：根据笛卡尔速度(xdot_ref)和当前关节角度(q_out)，计算出对应的关节速度(qdot_out)
        int ik_vel_ret = arm_model.ik_solver_vel_wdls->CartToJnt(q_out, xdot_ref, qdot_out);

        // bool point_valid = arm_model.comput_ik(x_ref);
        bool point_valid = true; // 暂时跳过 comput_ik 中的额外检查，避免误报超出工作空间

        // 3. 处理求解失败的情况
        if (ik_pos_ret < 0 || ik_vel_ret < 0 || !point_valid)
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
        x_ref_trajectory_.push_back(x_ref);       // 将计算出的笛卡尔位置存入轨迹列表
        xdot_ref_trajectory_.push_back(xdot_ref); // 将计算出的笛卡尔速度存入轨迹列表

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

    // publish_trajectory_visualization(arm_model);

    return true;
}

// 力位混合控制：某一个方向使用力控制，其他方向使用位置控制，通过掩码来区分控制的方向
void controller::HybridForceControl(nb_arm_xdof &arm_model, damiao::Motor_Control &motor)
{
    static float refer_mask[6] = {0};
    float end_effector_force[6] = {0};

    arm_model.computeInverseDynamics(
        arm_model.current_joint_positions,
        arm_model.current_joint_velocities,
        arm_model.current_joint_acceleration,
        arm_model.forward_joint_torques);
    for (size_t i = 0; i < 6; i++)
    {
        refer_mask[i] = (arm_model.end_force_inv[i] == 0) ? 1 : 0;
    }
    for (size_t i = 0; i < 3; i++)
    {
        arm_model.desire_end_effector_frame.p.data[i] = arm_model.desire_end_effector_frame.p.x() * refer_mask[i] + arm_model.current_end_effector_frame.p.x() * arm_model.end_force_inv[i];
    }
    double desire_roll, desire_pitch, desire_yaw, current_roll, current_pitch, current_yaw;
    arm_model.desire_end_effector_frame.M.GetRPY(desire_roll, desire_pitch, desire_yaw);
    arm_model.current_end_effector_frame.M.GetRPY(current_roll, current_pitch, current_yaw);

    float R_desire = desire_roll * refer_mask[3] + current_roll * arm_model.end_force_inv[3];
    float P_desire = desire_pitch * refer_mask[4] + current_pitch * arm_model.end_force_inv[4];
    float Y_desire = desire_yaw * refer_mask[5] + current_yaw * arm_model.end_force_inv[5];

    arm_model.desire_end_effector_frame.M = KDL::Rotation::RPY(R_desire, P_desire, Y_desire);
    int ret = arm_model.ik_solver_pos_lma->CartToJnt(arm_model.current_joint_positions, arm_model.desire_end_effector_frame, arm_model.desire_joint_positions);
    if (ret != 0)
    {
        std::cerr << "HybridForceControl 中 ik_solver_pos_lma->CartToJnt 计算失败, 错误码 = " << ret << std::endl;
        return;
    }
    for (size_t i = 0; i < 6; i++)
    {
        float component = 0.0f;
        for (size_t j = 0; j < ARM_DOF; j++)
        {
            component += arm_model.current_jacobian(i, j) * arm_model.current_joint_velocities(j);
        }
        // 前 3 行对应线速度，后 3 行对应角速度
        if (i < 3)
        {
            arm_model.current_end_effector_dot.vel(i) = component;
        }
        else
        {
            arm_model.current_end_effector_dot.rot(i - 3) = component;
        }
    }
    for (size_t i = 0; i < 6; i++)
    {
        end_effector_force[i] = arm_model.end_force_inv[i] * arm_model.end_force_value;
    }
    Eigen::MatrixXd J_transpose = arm_model.current_jacobian.data.transpose();
    Eigen::VectorXd end_effector_force_eigen(6);
    Eigen::VectorXd joint_torques_eigen(ARM_DOF);
    for (size_t i = 0; i < 6; i++)
    {
        end_effector_force_eigen(i) = end_effector_force[i];
    }
    joint_torques_eigen = J_transpose * end_effector_force_eigen;
    for (size_t i = 0; i < ARM_DOF; i++)
    {
        arm_model.joint_hybrid_tor[i] = joint_torques_eigen(i);
    }

    for (size_t i = 0; i < ARM_DOF; i++)
    {
        arm_model.joint_ctrl_tor[i] = arm_model.forward_joint_torques(i) + arm_model.joint_hybrid_tor[i];
    }

    for (size_t i = 0; i < ARM_DOF; i++)
    {
        arm_model.desire_joint_pos[i] = arm_model.desire_joint_positions(i);
        arm_model.desire_joint_vel[i] = 0.0;
    }
    arm_model.joint2motor(
        arm_model.desire_joint_pos,
        arm_model.desire_joint_vel,
        arm_model.desire_joint_acc,
        arm_model.joint_ctrl_tor,
        motor.desire_motor_pos,
        motor.desire_motor_vel,
        motor.desire_motor_acc,
        motor.desire_motor_tor);

    motor.mit_kp[0] = 35.0;
    motor.mit_kd[0] = 1.51;

    motor.mit_kp[1] = 27.0;
    motor.mit_kd[1] = 2.01;

    motor.mit_kp[2] = 27.0;
    motor.mit_kd[2] = 2.01;

    motor.mit_kp[3] = 55.0;
    motor.mit_kd[3] = 0.9;

    motor.mit_kp[4] = 20.0;
    motor.mit_kd[4] = 0.51;

    motor.mit_kp[5] = 10.0;
    motor.mit_kd[5] = 0.51;

    motor.CtrlMotors(
        motor.desire_motor_pos,
        motor.desire_motor_vel,
        motor.mit_kp,
        motor.mit_kd,
        motor.desire_motor_tor);
}

// 加入前馈力矩补偿的位控
// 这部分是基于空间尺度的轨迹跟踪，可能效果不会理想，暂时注释，如果基于时间尺度的轨迹跟踪效果不好再尝试这种方式
// void controller::PositionControl(nb_arm_xdof &arm_model, motor_driver &motor)
// {
//     // 测试用预设路径点
//     const std::vector<KDL::Frame> test_waypoints = {
//         KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.2, 0)),
//         KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.3, 0.5, 0.1)),
//         KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.25, 0.5)),
//         KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.2, 0.2, 0.2)),
//     };

//     auto path = traj_gen.generate_rounded_path(test_waypoints); // 圆角路径
//     // auto path = traj_gen.generate_line_path(test_waypoints);  // 直线路径

//     // 生成轨迹并预计算
//     // bool trajectory_valid = get_trajectory(arm_model, traj_gen, std::move(path), true, 5.0);  // 梯形速度
//     bool trajectory_valid = get_trajectory(arm_model, traj_gen, std::move(path), false, 5.0); // 样条插值速度

//     if (!trajectory_valid)
//     {
//         std::cerr << "PositionControl: 轨迹生成失败" << std::endl;
//         return;
//     }

//     size_t trajectory_size = q_ref_trajectory_.size();
//     if (trajectory_size == 0)
//         return;
//     arm_model.update_armState(
//         motor.current_motor_pos,
//         motor.current_motor_vel,
//         motor.current_motor_tor);
//     const auto loop_period = std::chrono::milliseconds(static_cast<long>(1000.0 / control_frequency));
//     std::cout << "开始执行控制循环..." << std::endl;
//     // 重置索引
//     last_closest_index_ = 0;
//     const int lookahead_points = 10;        // 前瞻点
//     size_t target_index = lookahead_points; // 在循环外初始化目标索引
//     const float max_run_time = 5000.0;    // [毫秒] 最大允许运行时间，防止死循环
//     auto start_time = std::chrono::steady_clock::now();
//     auto next_cycle_time = start_time;

//     while (last_closest_index_ < trajectory_size - 1)
//     {
//         {
//             std::lock_guard<std::mutex> lockf(data_mutex);
//             arm_model.update_armState(
//                 motor.current_motor_pos,
//                 motor.current_motor_vel,
//                 motor.current_motor_tor);
//             arm_model.jac_solver->JntToJac(arm_model.current_joint_positions, arm_model.current_jacobian);
//         }

//         // 1. 寻找最近点 (基于关节空间距离)
//         // 为了提高效率，从上一次的最近点开始向后搜索
//         float min_dist_sq = std::numeric_limits<float>::max();
//         size_t closest_index = last_closest_index_;

//         // 搜索窗口大小，避免每次都遍历整个轨迹
//         size_t search_window = 50;
//         size_t search_end = std::min(last_closest_index_ + search_window, trajectory_size);

//         for (size_t i = last_closest_index_; i < search_end; ++i)
//         {
//             float dist_sq = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[i]);
//             if (dist_sq < min_dist_sq)
//             {
//                 min_dist_sq = dist_sq;
//                 closest_index = i;
//             }
//         }
//         last_closest_index_ = closest_index;

//         // 2. 确定目标点 (Lookahead)
//         // 选择最近点前方一定距离的点作为当前控制周期的目标点
//         target_index = std::min(closest_index + lookahead_points, trajectory_size - 1);

//         // 检查是否到达当前目标点
//         // float distance_to_target = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[target_index]);
//         // const float arrival_threshold = 0.01; // 到达阈值，需要根据实际情况调整
//         // if (distance_to_target < arrival_threshold)
//         // {
//         //     // 到达后，更新到下一个目标点
//         //     target_index = std::min(closest_index + lookahead_points, trajectory_size - 1);
//         // }

//         // 3. 获取目标状态
//         const KDL::JntArray &q_ref = q_ref_trajectory_[target_index];
//         const KDL::JntArray &qdot_ref = qdot_ref_trajectory_[target_index];
//         const KDL::JntArray &qddot_ref = qddot_ref_trajectory_[target_index];

//         // 4. 计算前馈力矩 (Inverse Dynamics)
//         KDL::JntArray tor_ff(ARM_DOF);
//         arm_model.computeInverseDynamics(q_ref, qdot_ref, qddot_ref, tor_ff);

//         // 5. 更新模型期望状态
//         arm_model.desire_joint_positions = q_ref;
//         arm_model.desire_joint_velocities = qdot_ref;
//         arm_model.desire_joint_acceleration = qddot_ref;

//         // 6. 转换为电机指令 (MIT模式: 前馈力矩 + PD控制)
//         float q_ref_arr[ARM_DOF];
//         float qdot_ref_arr[ARM_DOF];
//         float qddot_ref_arr[ARM_DOF];
//         float tor_ff_arr[ARM_DOF];

//         for (int i = 0; i < ARM_DOF; ++i)
//         {
//             q_ref_arr[i] = q_ref(i);
//             qdot_ref_arr[i] = qdot_ref(i);
//             qddot_ref_arr[i] = qddot_ref(i);
//             tor_ff_arr[i] = tor_ff(i);
//         }

//         {
//             std::lock_guard<std::mutex> lockf(data_mutex);
//             // 设置PD参数 (可以根据需要调整)
//             motor.mit_kp[0] = 35.0;
//             motor.mit_kd[0] = 1.51;

//             motor.mit_kp[1] = 27.0;
//             motor.mit_kd[1] = 2.01;

//             motor.mit_kp[2] = 27.0;
//             motor.mit_kd[2] = 2.01;

//             motor.mit_kp[3] = 55.0;
//             motor.mit_kd[3] = 0.9;

//             motor.mit_kp[4] = 20.0;
//             motor.mit_kd[4] = 0.51;

//             motor.mit_kp[5] = 10.0;
//             motor.mit_kd[5] = 0.51;

//             arm_model.joint2motor(
//                 q_ref_arr,
//                 qdot_ref_arr,
//                 qddot_ref_arr,
//                 tor_ff_arr,
//                 motor.desire_motor_pos,
//                 motor.desire_motor_vel,
//                 motor.desire_motor_acc,
//                 motor.desire_motor_tor);
//         }
//         // 7. 发送指令给电机
//         motor.CtrlMotors(
//             motor.desire_motor_pos,
//             motor.desire_motor_vel,
//             motor.mit_kp,
//             motor.mit_kd,
//             motor.desire_motor_tor);

//         // 超时检查
//         auto now = std::chrono::steady_clock::now();
//         if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count() > max_run_time)
//         {
//             std::cerr << "PositionControl: 执行超时，强制退出" << std::endl;
//             break;
//         }

//         next_cycle_time += loop_period;
//         std::this_thread::sleep_until(next_cycle_time);
//         // 检查是否到达终点
//         if (closest_index >= trajectory_size - 1)
//         {
//             std::cout << "PositionControl: 已到达轨迹终点" << std::endl;
//         }
//     }
// }

// 加入前馈力矩补偿的位控
// 基于时间尺度的轨迹跟踪
void controller::PositionControl(nb_arm_xdof &arm_model, damiao::Motor_Control &motor)
{
    // 测试用预设路径点
    const std::vector<KDL::Frame> test_waypoints = {
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.2, 0)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.3, 0.5, 0.1)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.25, 0.25, 0.5)),
        KDL::Frame(KDL::Rotation::RPY(0, 0, 0), KDL::Vector(0.2, 0.2, 0.2)),
    };

    // 以上只是测试用的路径点，实际执行任务的时候使用外部得到的path，这部分一定要注释掉！！！！！！！！！！！！！！！

    auto path = traj_gen.generate_rounded_path(test_waypoints); // 圆角路径
    // auto path = traj_gen.generate_line_path(test_waypoints);  // 直线路径

    // 生成轨迹并预计算
    // bool trajectory_valid = get_trajectory(arm_model, traj_gen, std::move(path), true, 5.0);  // 梯形速度
    bool trajectory_valid = get_trajectory(arm_model, traj_gen, std::move(path), false, 5.0); // 样条插值速度

    if (!trajectory_valid)
    {
        std::cerr << "PositionControl: 轨迹生成失败" << std::endl;
        return;
    }

    size_t trajectory_size = q_ref_trajectory_.size();
    if (trajectory_size == 0)
        return;

    float traj_dt = 1.0 / control_frequency;
    // 更新一次机械臂状态
    {
        std::lock_guard<std::mutex> lockf(data_mutex);
        arm_model.update_armState(
            motor.current_motor_pos,
            motor.current_motor_vel,
            motor.current_motor_tor);
    }

    // 防止机械臂当前位置和轨迹起点相差太远导致猛冲
    float start_error = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[0]);
    if (start_error > 0.1)
    { // 阈值需根据实际情况调整
        std::cerr << "错误: 当前位置距离轨迹起点过远，禁止启动！" << std::endl;
        // return; // 暂时注释掉，方便测试
    }

    const auto loop_period = std::chrono::milliseconds(static_cast<long>(1000.0 / control_frequency));
    std::cout << "开始执行基于时间尺度的轨迹跟踪..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto next_cycle_time = start_time;

    bool finished = false;

    while (!finished)
    {
        // 1. 获取当前时间的准确流逝量 (Time Elapsed)
        auto now = std::chrono::steady_clock::now();
        float t_elapsed = std::chrono::duration<float>(now - start_time).count();

        // 2. 根据时间计算当前的索引 (Index)
        // 这种写法保证了即使系统卡顿，也会跳过几个点来追赶时间，保证前馈力矩的物理意义正确
        size_t i = static_cast<size_t>(t_elapsed / traj_dt);

        // 3. 检查是否结束
        if (i >= trajectory_size)
        {
            i = trajectory_size - 1; // 保持在最后一个点
            finished = true;         // 标记结束，或者你可以选择在这里 break
        }
        {
            std::lock_guard<std::mutex> lockf(data_mutex);
            arm_model.update_armState(motor.current_motor_pos, motor.current_motor_vel, motor.current_motor_tor);
        }

        float tracking_error = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[i]);
        if (tracking_error > 0.225f)
        { // 这里的 0.225 是距离平方，根据实际单位调整
            std::cerr << "错误: 跟踪误差过大 (堵转或失控)，触发急停！" << std::endl;
            // 发送零力矩或急停指令
            motor.disable_motor();
            break;
        }

        const KDL::JntArray &q_ref = q_ref_trajectory_[i];
        const KDL::JntArray &qdot_ref = qdot_ref_trajectory_[i];
        const KDL::JntArray &qddot_ref = qddot_ref_trajectory_[i];

        KDL::JntArray tor_ff(arm_model.kdl_chain.getNrOfJoints());
        arm_model.computeInverseDynamics(q_ref, qdot_ref, qddot_ref, tor_ff);

        // 4. 更新模型期望状态
        arm_model.desire_joint_positions = q_ref;
        arm_model.desire_joint_velocities = qdot_ref;
        arm_model.desire_joint_acceleration = qddot_ref;

        // 5. 准备电机指令数据
        float q_ref_arr[ARM_DOF];
        float qdot_ref_arr[ARM_DOF];
        float qddot_ref_arr[ARM_DOF];
        float tor_ff_arr[ARM_DOF];

        for (int j = 0; j < ARM_DOF; ++j)
        {
            q_ref_arr[j] = q_ref(j);
            qdot_ref_arr[j] = qdot_ref(j);
            qddot_ref_arr[j] = qddot_ref(j);
            tor_ff_arr[j] = tor_ff(j);
        }

        {
            std::lock_guard<std::mutex> lockf(data_mutex);
            // 设置PD参数 (可以根据需要调整)
            motor.mit_kp[0] = 35.0;
            motor.mit_kd[0] = 1.51;

            motor.mit_kp[1] = 27.0;
            motor.mit_kd[1] = 2.01;

            motor.mit_kp[2] = 27.0;
            motor.mit_kd[2] = 2.01;

            motor.mit_kp[3] = 55.0;
            motor.mit_kd[3] = 0.9;

            motor.mit_kp[4] = 20.0;
            motor.mit_kd[4] = 0.51;

            motor.mit_kp[5] = 10.0;
            motor.mit_kd[5] = 0.51;

            arm_model.joint2motor(
                q_ref_arr,
                qdot_ref_arr,
                qddot_ref_arr,
                tor_ff_arr,
                motor.desire_motor_pos,
                motor.desire_motor_vel,
                motor.desire_motor_acc,
                motor.desire_motor_tor);
        }
        // 6. 发送指令给电机
        motor.CtrlMotors(
            motor.desire_motor_pos,
            motor.desire_motor_vel,
            motor.mit_kp,
            motor.mit_kd,
            motor.desire_motor_tor);

        if (finished)
            break;

        // 9. 频率控制
        next_cycle_time += loop_period;
        std::this_thread::sleep_until(next_cycle_time);
    }

    std::cout << "PositionControl: 轨迹执行完毕" << std::endl;
    modeSwitch(State::Zero); // 轨迹执行完毕后切换到 Zero 状态，防止重复执行
}

void controller::go_home(nb_arm_xdof &arm_model, damiao::Motor_Control &motor)
{
    // 实现机械臂回到初始位置的控制逻辑
    std::cout << "执行 go_home 控制..." << std::endl;
    KDL::Frame home_pose;
    int ret = arm_model.fk_solver_pos->JntToCart(arm_model.home_joint_positions, home_pose);
    std::cout << "home position: "
              << home_pose.p.x() << ", "
              << home_pose.p.y() << ", "
              << home_pose.p.z() << std::endl;
    if (ret != 0)
    {
        std::cerr << "go_home 中 fk_solver_pos->JntToCart 计算失败, 错误码 = " << ret << std::endl;
        return;
    }
    KDL::Frame current_pose;
    arm_model.update_armState(
        motor.current_motor_pos,
        motor.current_motor_vel,
        motor.current_motor_tor);
    int ret_current = arm_model.fk_solver_pos->JntToCart(arm_model.current_joint_positions, current_pose);
    std::cout << "current position: "
              << current_pose.p.x() << ", "
              << current_pose.p.y() << ", "
              << current_pose.p.z() << std::endl;
    if (ret_current != 0)
    {
        std::cerr << "go_home 中 fk_solver_pos->JntToCart 计算失败, 错误码 = " << ret_current << std::endl;
        return;
    }
    std::vector<KDL::Frame> home_waypoints = {current_pose, home_pose};
    auto path = traj_gen.generate_line_path(home_waypoints);                                  // 直线路径
    bool trajectory_valid = get_trajectory(arm_model, traj_gen, std::move(path), false, 5.0); // 样条插值速度

    if (!trajectory_valid)
    {
        std::cerr << "gohome: 轨迹生成失败" << std::endl;
        return;
    }

    size_t trajectory_size = q_ref_trajectory_.size();
    if (trajectory_size == 0)
        return;

    float traj_dt = 1.0 / control_frequency;
    // 更新一次机械臂状态
    {
        std::lock_guard<std::mutex> lockf(data_mutex);
        arm_model.update_armState(
            motor.current_motor_pos,
            motor.current_motor_vel,
            motor.current_motor_tor);
    }

    // // 防止机械臂当前位置和轨迹起点相差太远导致猛冲
    // float start_error = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[0]);
    // if (start_error > 0.1f)
    // { // 阈值需根据实际情况调整
    //     std::cerr << "错误: (go_home)当前位置距离轨迹起点过远，禁止启动！" << std::endl;
    //     return;
    // }

    const auto loop_period = std::chrono::milliseconds(static_cast<long>(1000.0 / control_frequency));
    std::cout << "开始执行基于时间尺度的轨迹跟踪..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    auto next_cycle_time = start_time;

    bool finished = false;

    while (!finished)
    {
        // 1. 获取当前时间的准确流逝量 (Time Elapsed)
        auto now = std::chrono::steady_clock::now();
        float t_elapsed = std::chrono::duration<float>(now - start_time).count();

        // 2. 根据时间计算当前的索引 (Index)
        // 这种写法保证了即使系统卡顿，也会跳过几个点来追赶时间，保证前馈力矩的物理意义正确
        size_t i = static_cast<size_t>(t_elapsed / traj_dt);

        // 3. 检查是否结束
        if (i >= trajectory_size)
        {
            i = trajectory_size - 1; // 保持在最后一个点
            finished = true;         // 标记结束，或者你可以选择在这里 break
        }
        {
            std::lock_guard<std::mutex> lockf(data_mutex);
            arm_model.update_armState(motor.current_motor_pos, motor.current_motor_vel, motor.current_motor_tor);
        }

        // float tracking_error = distanceSq(arm_model.current_joint_positions, q_ref_trajectory_[i]);
        // if (tracking_error > 0.225)
        // { // 这里的 0.225 是距离平方，根据实际单位调整
        //     std::cerr << "错误: 跟踪误差过大 (堵转或失控)，触发急停！" << std::endl;
        //     // 发送零力矩或急停指令
        //     motor.disable_motor();
        //     break;
        // }

        const KDL::JntArray &q_ref = q_ref_trajectory_[i];
        const KDL::JntArray &qdot_ref = qdot_ref_trajectory_[i];
        const KDL::JntArray &qddot_ref = qddot_ref_trajectory_[i];

        KDL::JntArray tor_ff(arm_model.kdl_chain.getNrOfJoints());
        arm_model.computeInverseDynamics(q_ref, qdot_ref, qddot_ref, tor_ff);
        // std::cout << "go_home: 当前点 " << i << " / " << trajectory_size - 1 << std::endl;

        // 4. 更新模型期望状态
        arm_model.desire_joint_positions = q_ref;
        arm_model.desire_joint_velocities = qdot_ref;
        arm_model.desire_joint_acceleration = qddot_ref;

        // 5. 准备电机指令数据
        float q_ref_arr[ARM_DOF];
        float qdot_ref_arr[ARM_DOF];
        float qddot_ref_arr[ARM_DOF];
        float tor_ff_arr[ARM_DOF];

        for (int j = 0; j < ARM_DOF; ++j)
        {
            q_ref_arr[j] = q_ref(j);
            qdot_ref_arr[j] = qdot_ref(j);
            qddot_ref_arr[j] = qddot_ref(j);
            tor_ff_arr[j] = tor_ff(j);
            // std::cout << "go_home: tor_ff_arr[" << j << "] = " << tor_ff_arr[j] << std::endl;
        }

        {
            // std::cout << "go_home: 发送第 " << i << " 个点指令" << std::endl;
            std::lock_guard<std::mutex> lockf(data_mutex);
            // 设置PD参数 (可以根据需要调整)
            motor.mit_kp[0] = 35.0;
            motor.mit_kd[0] = 1.51;

            motor.mit_kp[1] = 20.0;
            motor.mit_kd[1] = 1.01;

            motor.mit_kp[2] = 27.0;
            motor.mit_kd[2] = 2.01;

            motor.mit_kp[3] = 55.0;
            motor.mit_kd[3] = 0.9;

            motor.mit_kp[4] = 20.0;
            motor.mit_kd[4] = 0.51;

            motor.mit_kp[5] = 10.0;
            motor.mit_kd[5] = 0.51;

            arm_model.joint2motor(
                q_ref_arr,
                qdot_ref_arr,
                qddot_ref_arr,
                tor_ff_arr,
                motor.desire_motor_pos,
                motor.desire_motor_vel,
                motor.desire_motor_acc,
                motor.desire_motor_tor);
            // std::cout << "go_home: 计算电机指令完成" << std::endl;
        }
        // 6. 发送指令给电机
        motor.CtrlMotors(
            motor.desire_motor_pos,
            motor.desire_motor_vel,
            motor.mit_kp,
            motor.mit_kd,
            motor.desire_motor_tor);
        // std::cout << "go_home: 发送电机指令完成" << std::endl;
        if (finished)
            break;

        // 9. 频率控制
        next_cycle_time += loop_period;
        std::this_thread::sleep_until(next_cycle_time);
    }

    std::cout << "go_home: 轨迹执行完毕" << std::endl;
    modeSwitch(State::Zero); // 轨迹执行完毕后切换到 Zero 状态，防止重复执行
}