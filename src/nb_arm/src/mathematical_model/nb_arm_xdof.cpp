#include "nb_arm/mathematical_model/nb_arm_xdof.hpp"

nb_arm_xdof::nb_arm_xdof()
{
    // Default constructor does nothing or sets default values
}

void nb_arm_xdof::init(const std::string &urdf_file)
{
    // 加载URDF模型
    // std::string urdf_file = "src/nb_arm/urdf/ur5.urdf"; // Removed hardcoded path

    urdf::Model model;
    if (!model.initFile(urdf_file))
    {
        throw std::runtime_error("Failed to load URDF file: " + urdf_file);
    }
    std::cout << "URDF model loaded successfully." << std::endl;
    std::cout << urdf_file << std::endl;
    // 解析URDF并生成KDL树
    KDL::Tree kdl_tree;
    if (!kdl_parser::treeFromUrdfModel(model, kdl_tree))
    {
        throw std::runtime_error("Failed to construct KDL tree");
    }
    std::cout << "KDL tree constructed successfully." << std::endl;

    // Debug: Print tree info
    std::cout << "Tree has " << kdl_tree.getNrOfSegments() << " segments and " << kdl_tree.getNrOfJoints() << " joints." << std::endl;
    KDL::SegmentMap segment_map = kdl_tree.getSegments();
    for (auto const& [name, segment] : segment_map)
    {
        std::cout << "Segment: " << name << " (Children: ";
        for (auto const& child : segment.children) {
            std::cout << child->first << " ";
        }
        std::cout << ")" << std::endl;
    }

    // 从KDL树生成KDL链
    if (!kdl_tree.getChain("base_link", "link6", kdl_chain)) // 根据实际的urdf链名称修改
    {
        throw std::runtime_error("Failed to get KDL chain");
    }

    arm_link_length.resize(kdl_chain.getNrOfSegments());
    total_arm_length = 0.0;
    for (size_t i = 0; i < kdl_chain.getNrOfSegments(); ++i)
    {
        const KDL::Segment &segment = kdl_chain.getSegment(i);
        const KDL::Frame &frame = segment.getFrameToTip();
        float length = frame.p.Norm(); // 获取该段的长度
        std::cout << "Segment " << i << " length: " << length << " meters" << std::endl;
        arm_link_length[i] = length;
        total_arm_length += length;
    }

    // 创建一个正向位置动力学求解器，用于计算给定关节角度下的末端执行器位置
    fk_solver_pos = std::make_shared<KDL::ChainFkSolverPos_recursive>(kdl_chain);
    // 创建一个正向速度动力学求解器，用于计算给定关节速度下的末端执行器速度
    fk_solver_vel = std::make_shared<KDL::ChainFkSolverVel_recursive>(kdl_chain);
    // 创建一个雅可比矩阵求解器，用于将关节速度转换为末端执行器速度
    jac_solver = std::make_shared<KDL::ChainJntToJacSolver>(kdl_chain);
    // 创建一个雅可比矩阵导数求解器，用于计算雅可比矩阵关于时间的导数从而用于加速度计算
    ik_solver_acc = std::make_shared<KDL::ChainJntToJacDotSolver>(kdl_chain);
    // 创建一个逆位置动力学求解器，使用Levenberg-Marquardt算法来求解关节角度
    ik_solver_pos_lma = std::make_shared<KDL::ChainIkSolverPos_LMA>(kdl_chain, _L);
    // 创建一个逆速度动力学求解器，使用加权最小二乘法来求解关节速度
    ik_solver_vel_wdls = std::make_shared<KDL::ChainIkSolverVel_wdls>(kdl_chain);
    // 创建一个动力学参数求解器，用于计算给定关节角度下的动力学参数，如惯性矩阵、科里奥利力和重力力
    dyn_solver = std::make_shared<KDL::ChainDynParam>(kdl_chain, KDL::Vector(0.0, 0.0, -9.81));

    id_solver = std::make_shared<KDL::ChainIdSolver_RNE>(kdl_chain, KDL::Vector(0.0, 0.0, -9.81));
    std::cout << "All KDL solvers initialized successfully." << std::endl;

    // 初始化当前关节位置、速度、扭矩和加速度数组
    current_joint_positions = KDL::JntArray(kdl_chain.getNrOfJoints());
    current_joint_velocities = KDL::JntArray(kdl_chain.getNrOfJoints());
    current_joint_torques = KDL::JntArray(kdl_chain.getNrOfJoints());
    current_joint_acceleration = KDL::JntArray(kdl_chain.getNrOfJoints());
    current_q_pos_vel = KDL::JntArrayVel(kdl_chain.getNrOfJoints()); // 使用arrayvel同时存储位置和速度

    desire_joint_positions = KDL::JntArray(kdl_chain.getNrOfJoints());
    desire_joint_velocities = KDL::JntArray(kdl_chain.getNrOfJoints());
    desire_joint_torques = KDL::JntArray(kdl_chain.getNrOfJoints());
    forward_joint_torques = KDL::JntArray(kdl_chain.getNrOfJoints());
    desire_joint_acceleration = KDL::JntArray(kdl_chain.getNrOfJoints());
    desire_q_pos_vel = KDL::JntArrayVel(kdl_chain.getNrOfJoints()); // 使用arrayvel同时存储位置和速度

    home_joint_positions = KDL::JntArray(kdl_chain.getNrOfJoints());

    // 将期望关节位置、速度和扭矩数组初始化为零
    desire_joint_positions.data.setZero();
    desire_joint_velocities.data.setZero();
    desire_joint_torques.data.setZero();
    desire_joint_acceleration.data.setZero();

    // 将当前关节位置、速度、扭矩和加速度数组初始化为零
    current_joint_positions.data.setZero();
    current_joint_velocities.data.setZero();
    current_joint_torques.data.setZero();
    current_joint_acceleration.data.setZero();

    home_joint_positions.data << 0, -1, 1, 0, 0, 0; //////////////     一定要注意修改这个初始位置！！！    ///////////////////

    // 初始化当前和期望的雅可比矩阵
    current_jacobian = KDL::Jacobian(kdl_chain.getNrOfJoints());
    desire_jacobian = KDL::Jacobian(kdl_chain.getNrOfJoints());
    // 初始化末端执行器的期望位置和姿态
    desire_end_effector_frame.p = KDL::Vector(desire_end_effector_pos_default[0], desire_end_effector_pos_default[1], desire_end_effector_pos_default[2]); // 末端期望空间位置
    desire_end_effector_frame.M = KDL::Rotation::RPY(0.0, 0 + 0.0, 0.0);                                                                                   // 末端期望空间姿态

    for (size_t i = 0; i < kdl_chain.getNrOfJoints(); i++)
    {
        // 关节位置
        desire_q_pos_vel.q(i) = 0.0;
        // 关节速度
        desire_q_pos_vel.qdot(i) = 0.0;

        // 关节位置
        current_q_pos_vel.q(i) = 0.0;
        // 关节速度
        current_q_pos_vel.qdot(i) = 0.0;
    }

    // 初始化期望末端执行器的速度和旋转为零向量
    desire_end_effector_dot.vel = KDL::Vector(0.0, 0.0, 0.0); // dot速度
    desire_end_effector_dot.rot = KDL::Vector(0.0, 0.0, 0.0);
    // 初始化当前末端执行器的加速度和旋转加速度为零向量
    current_end_effector_ddot.vel = KDL::Vector(0.0, 0.0, 0.0); // ddot加速度
    current_end_effector_ddot.rot = KDL::Vector(0.0, 0.0, 0.0);
    // 初始化期望末端执行器的力和扭矩为零向量
    desire_end_effector_wrench.force = KDL::Vector(0, 0, 0);
    desire_end_effector_wrench.torque = KDL::Vector(0, 0, 0);
    // 初始化当前末端执行器的力和扭矩为零向量
    current_end_effector_wrench.force = KDL::Vector(0, 0, 0);
    current_end_effector_wrench.torque = KDL::Vector(0, 0, 0);
}

nb_arm_xdof::~nb_arm_xdof() {}

/**
 * @brief 计算逆动力学
 * @param q 关节位置
 * @param q_dot 关节速度
 * @param q_dotdot 关节加速度
 * @param torques 输出各个关节的力矩
 * 分成 M C G 三部分
 * 考虑的受外力的情况，函数内部外力为0
 */
void nb_arm_xdof::computeInverseDynamics(const KDL::JntArray &q, const KDL::JntArray &q_dot, const KDL::JntArray &q_dotdot, KDL::JntArray &torques)
{
    // // 计算重力力矩
    // KDL::JntArray gravity_torques(kdl_chain.getNrOfJoints());
    // // 计算科里奥利力矩
    // KDL::JntArray coriolis_torques(kdl_chain.getNrOfJoints());
    // // 计算惯性力矩
    // KDL::JntArray mass_torques(kdl_chain.getNrOfJoints());
    // // 定义关节空间的惯性矩阵
    // KDL::JntSpaceInertiaMatrix inertia_matrix(kdl_chain.getNrOfJoints());

    // KDL::Jacobian jacobian_at_q(kdl_chain.getNrOfJoints());

    // // 计算关节位置 q 下的重力力矩
    // dyn_solver->JntToGravity(q, gravity_torques); // dyn_solver动力学参数求解器，已在构造函数中初始化
    // // 计算关节位置 q 和速度 q_dot 下的科里奥利力矩
    // dyn_solver->JntToCoriolis(q, q_dot, coriolis_torques);
    // // 计算关节位置 q 下的惯性矩阵 M
    // dyn_solver->JntToMass(q, inertia_matrix); // 惯性矩阵M

    // // 设定末端外力：例如沿 X 轴的一个力（单位：N），无外部力矩
    // external_force[2] = 0;

    // // 计算外力对关节的影响：tau_ext = J^T * f_ext
    // KDL::JntArray tor_ext(kdl_chain.getNrOfJoints());
    // jac_solver->JntToJac(q, jacobian_at_q);
    // // KDL::Wrench 内部存储为一个 6 维变量：前3个为力，后3个为力矩
    // // 把末端力转换到关节力
    // for (size_t i = 0; i < kdl_chain.getNrOfJoints(); i++)
    // {
    //     tor_ext(i) = 0.0;
    //     for (size_t j = 0; j < 6; j++)
    //     {
    //         tor_ext(i) += jacobian_at_q(j, i) * external_force[j]; // 通过索引来使用转置之后的雅各比矩阵，无需求转置矩阵，
    //                                                                // 计算末端力矩需要求伪逆矩阵，所以必须要转置
    //     }
    // }

    // for (size_t i = 0; i < kdl_chain.getNrOfJoints(); i++)
    // {
    //     float inertia_term = 0.0;
    //     // 对每一行求和，考虑所有关节的耦合效应
    //     for (size_t j = 0; j < kdl_chain.getNrOfJoints(); j++)
    //     {
    //         inertia_term += inertia_matrix(i, j) * q_dotdot(j);
    //     }
    //     // 将惯性项、Coriolis项、重力项相加，再减去外力影响（tau_ext）
    //     torques(i) = inertia_term + coriolis_torques(i) + gravity_torques(i) - tor_ext(i); // 加减末端外力根据方向判断
    // }

    // 1. 检查求解器是否已初始化
    if (!id_solver)
    {
        std::cerr << "Error: Inverse dynamics solver (RNE) 初始化失败" << std::endl;
        return;
    }

    // Debug sizes
    if (q.rows() != kdl_chain.getNrOfJoints() ||
        q_dot.rows() != kdl_chain.getNrOfJoints() ||
        q_dotdot.rows() != kdl_chain.getNrOfJoints() ||
        torques.rows() != kdl_chain.getNrOfJoints())
    {
        std::cerr << "Error: Size mismatch in computeInverseDynamics" << std::endl;
        std::cerr << "Chain Joints: " << kdl_chain.getNrOfJoints() << std::endl;
        std::cerr << "q: " << q.rows() << std::endl;
        std::cerr << "q_dot: " << q_dot.rows() << std::endl;
        std::cerr << "q_dotdot: " << q_dotdot.rows() << std::endl;
        std::cerr << "torques: " << torques.rows() << std::endl;
        return;
    }

    // 2. 准备求解器所需的外部力参数 (f_ext)，默认为0
    KDL::Wrenches f_ext(kdl_chain.getNrOfSegments());
    // KDL::Wrenches 的默认构造函数会将其所有元素初始化为零，所以不需要手动清零。

    // 3. 调用底层的RNE求解器
    // 将所有准备好的参数传递给求解器的 CartToJnt
    int ret = id_solver->CartToJnt(q, q_dot, q_dotdot, f_ext, torques);

    // 4. (推荐) 检查返回值以处理可能的错误
    if (ret < 0)
    {
        std::cerr << "Error: KDL::ChainIdSolver_RNE::CartToJnt 求解失败 " << ret << std::endl;
        // 在这里可以根据需要进行更详细的错误处理，例如抛出异常。
    }
}
void nb_arm_xdof::joint2end_vel()
{
    KDL::JntArrayVel jnt_pos_vel(kdl_chain.getNrOfJoints());
    jnt_pos_vel.q = current_joint_positions;
    jnt_pos_vel.qdot = current_joint_velocities;

    KDL::FrameVel end_frame_vel;

    int ret = fk_solver_vel->JntToCart(jnt_pos_vel, end_frame_vel);

    if (ret >= 0)
    {
        // 4. 从结果中提取速度（Twist）并赋值
        // frame_vel.GetTwist() 返回一个 KDL::Twist 对象
        current_end_effector_dot = end_frame_vel.GetTwist();
    }
    else
    {
        // 处理错误
        printf("Failed to calculate forward velocity kinematics, error code: %d\n", ret);
    }
}

/**
 * @brief 正运动学计算末端力矩
 * joint_torqe = J^T * F
 *
 * F = (J^T)^-1 * joint_torqe
 * @note 考虑静止状态的力矩计算
 * */
void nb_arm_xdof::joint2end_torque()
{
    // 计算 J^T 的伪逆
    Eigen::MatrixXd Jt = current_jacobian.data.transpose(); // 求雅各比矩阵的转置

    Eigen::MatrixXd Jt_pinv = Jt.completeOrthogonalDecomposition().pseudoInverse(); // 计算伪逆

    // 用伪逆计算末端力（wrench），结果是一个6维向量（前三个分量是力，后三个是力矩）
    // current_joint_torques是电机反馈回来的力矩，gravity_joint_torque是重力补偿力矩
    dyn_solver->JntToGravity(current_joint_positions, gravity_joint_torque);
    Eigen::VectorXd wrench = Jt_pinv * (current_joint_torques.data - gravity_joint_torque.data);

    // 如果需要，可以将 wrench 转换为 KDL::Wrench 类型：
    current_end_effector_wrench.force = KDL::Vector(wrench(0), wrench(1), wrench(2));

    current_end_effector_wrench.torque = KDL::Vector(wrench(3), wrench(4), wrench(5));
}

/**
 * @brief 计算关节的加速度
 * @param q 关节角度
 * @param q_dot 关节角速度
 * @param desir_end_effector_ddot 期望轨迹的加速度
 * @param q_dotdot 计算出来的关节加速度
 * */
void nb_arm_xdof::compute_joint_acceleration(const KDL::JntArrayVel &q_pos_vel, KDL::Twist &end_effector_ddot, KDL::JntArray &q_dotdot)
{
    // 调用 JntToJacDot 得到 dot(J)*qdot (存储在一个 Twist 中)
    KDL::Twist jac_dot_q_dot;

    int ret = ik_solver_acc->JntToJacDot(q_pos_vel, jac_dot_q_dot);

    if (ret != 0)
    {
        std::cerr << "JntToJacDot 计算失败, 错误码 = " << ret << std::endl;
        return;
    }

    // 计算关节加速度

    KDL::Twist delta_acc = end_effector_ddot - jac_dot_q_dot;

    ret = ik_solver_vel_wdls->CartToJnt(q_pos_vel.q, delta_acc, q_dotdot);

    if (ret != 0)
    {
        std::cerr << "ik_solver_vel 计算失败, 错误码 = " << ret << std::endl;
        return;
    }
}

/**
 * @brief 关节空间 -> 电机空间
 */
void nb_arm_xdof::joint2motor(float q[ARM_DOF], float dq[ARM_DOF], float ddq[ARM_DOF], float tor_joint[ARM_DOF], float motor_pos[ARM_DOF], float motor_vel[ARM_DOF], float motor_acc[ARM_DOF], float tor_motor[ARM_DOF])
{
    for (size_t i = 0; i < ARM_DOF; i++)
    {
        motor_pos[i] = motor_direction[i] * q[i];
        motor_vel[i] = motor_direction[i] * dq[i];
        motor_acc[i] = motor_direction[i] * ddq[i];
        // tor_motor[i] = motor_direction[i] * tor_joint[i];
        tor_motor[i] = 0 * motor_direction[i] * tor_joint[i]; // 先不考虑力矩控制
    }
}

/**
 * 在主函数实时的更新关节位置、速度、力矩
 */
void nb_arm_xdof::update_armState(float motor_pos[ARM_DOF], float motor_vel[ARM_DOF], float motor_tau[ARM_DOF])
{
    for (size_t i = 0; i < kdl_chain.getNrOfJoints(); i++)
    {
        current_joint_positions(i) = motor_direction[i] * motor_pos[i];
        current_joint_velocities(i) = motor_direction[i] * motor_vel[i];
        current_joint_torques(i) = motor_direction[i] * motor_tau[i];
    }
    for (size_t i = 0; i < kdl_chain.getNrOfJoints(); i++)
    {
        current_q_pos_vel.q(i) = current_joint_positions(i);
        current_q_pos_vel.qdot(i) = current_joint_velocities(i);
    }
    jac_solver->JntToJac(current_joint_positions, current_jacobian); // 更新雅各比矩阵
}

/**
 * @brief ik计算成功更新关节角度，失败则不更新。期望轨迹超出工作空间则不更新。
 * ik计算出的角度超出关节限制也不会更新关节角度
 */
bool nb_arm_xdof::comput_ik(KDL::Frame &frame)
{
    KDL::JntArray desir_joint_positions_temp(6);

    // fk_solver_pos->JntToCart(current_joint_positions, frame, 4);

    // 末端到（0,0,0.1）的距离
    float desir_arm_length = sqrt(frame.p.x() * frame.p.x() + frame.p.y() * frame.p.y() + (frame.p.z() - 0.1) * (frame.p.z() - 0.1));

    float arm_length = arm_link_length[2] + arm_link_length[3]; // 大臂长度+小臂长度

    if (arm_length < (desir_arm_length - 0.01))
    {
        std::cerr << "期望轨迹超出工作空间: " << std::endl;
        std::cerr << "arm_length:  " << arm_length << std::endl;
        std::cerr << "desir_arm_length:  " << desir_arm_length << std::endl;
        return false;
    }

    KDL::JntArray ik_init_joint_positions;

    ik_init_joint_positions = KDL::JntArray(6);

    ik_init_joint_positions.data.setZero();

    // IKD的初始位置可以用当前的关节角度也可以用初始关节角度（全零）
    int ret = ik_solver_pos_lma->CartToJnt(current_joint_positions, frame, desir_joint_positions_temp);

    // 注释的部分使用的是从初始位置开始算
    //  int ret = ik_solver_lma->CartToJnt(ik_init_joint_positions, desir_end_effector_frame, desir_joint_positions_temp);

    if (ret != 0)
    {
        std::cerr << "ik_solver_lma->CartToJnt 计算失败, 错误码 = " << ret << std::endl;
        return false;
    }
    else
    {

        for (size_t i = 0; i < 6; i++)
        {
            if (desir_joint_positions_temp(i) > desire_joint_pos_protect_max[i])
            {
                std::cerr << "关节 " << i << " 角度max超出工作空间" << desir_joint_positions_temp(i) << std::endl;
                return false;
            }
            if (desir_joint_positions_temp(i) < desire_joint_pos_protect_min[i])
            {
                std::cerr << "关节 " << i << " 角度min超出工作空间" << desir_joint_positions_temp(i) << std::endl;
                return false;
            }
        }

        // desire_joint_positions = desir_joint_positions_temp;
        return true;
    }
}

/**
 * @brief 更新惯性参数
 * @param chain 链
 * @param link_name 连杆名称
 * 用于估计末端负载
 */
void nb_arm_xdof::updateLinkMass(const std::string &link_name, float new_mass)
{

    KDL::Chain new_chain;
    for (unsigned int i = 0; i < kdl_chain.getNrOfSegments(); i++)
    {
        KDL::Segment seg = kdl_chain.getSegment(i);
        // 判断是否为需要更新的连杆
        if (seg.getName() == link_name)
        {
            // 获取原来的惯性参数
            KDL::RigidBodyInertia inertia = seg.getInertia();
            // 使用原有的质心和转动惯量，但更新质量
            KDL::RigidBodyInertia new_inertia(new_mass, inertia.getCOG(), inertia.getRotationalInertia());
            // 构造一个新的 Segment，其他参数与原来一致，只是 inertia 更新了
            KDL::Segment new_seg(seg.getName(), seg.getJoint(), seg.getFrameToTip(), new_inertia);
            new_chain.addSegment(new_seg);
            std::cout << "更新连杆 " << link_name << " 的质量为 " << new_mass << std::endl;
        }
        else
        {
            new_chain.addSegment(seg);
        }
    }
    // 用新的链替换旧的链
    kdl_chain = new_chain;
    std::cout << "连杆 " << link_name << " 的质量已更新为 " << new_mass << std::endl;

    // KDL链已更改，所有求解器都必须重新初始化！
    std::cout << "正在重新初始化所有KDL求解器..." << std::endl;
    fk_solver_pos = std::make_shared<KDL::ChainFkSolverPos_recursive>(kdl_chain);
    fk_solver_vel = std::make_shared<KDL::ChainFkSolverVel_recursive>(kdl_chain);
    jac_solver = std::make_shared<KDL::ChainJntToJacSolver>(kdl_chain);
    ik_solver_acc = std::make_shared<KDL::ChainJntToJacDotSolver>(kdl_chain);
    ik_solver_pos_lma = std::make_shared<KDL::ChainIkSolverPos_LMA>(kdl_chain, _L);
    ik_solver_vel_wdls = std::make_shared<KDL::ChainIkSolverVel_wdls>(kdl_chain);
    dyn_solver = std::make_shared<KDL::ChainDynParam>(kdl_chain, KDL::Vector(0.0, 0.0, -9.81));

    std::cout << "KDL求解器重新初始化完毕。" << std::endl;
}
