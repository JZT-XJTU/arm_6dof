#ifndef ARM_CONTROL_HPP
#define ARM_CONTROL_HPP

#include "nb_arm/mathematical_model/nb_arm_xdof.hpp"
#include "nb_arm/path/trajectory_generator.hpp"
#include "nb_arm/drivers/motor_driver.hpp"
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

class arm_control
{
public:
    arm_control(rclcpp::Node::SharedPtr node);
    ~arm_control() = default;
    bool get_trajectory(
        nb_arm_xdof &arm_model,
        trajectory_generator &traj_gen,
        std::unique_ptr<KDL::Path> path,
        bool is_trap,
        float spline_duration);
    // 导纳控制
    void admittance_control_update(
        nb_arm_xdof &arm_model,
        const KDL::JntArray &q_ref,
        const KDL::JntArray &qdot_ref,
        const KDL::JntArray &qddot_ref,
        float dt);

    float distanceSq(const KDL::JntArray &a, const KDL::JntArray &b);
    // 控制循环
    void control_loop(nb_arm_xdof &arm_model, motor_driver &motor);

    /**
     * @brief 将预计算的关节轨迹转换为笛卡尔路径并发布到RViz
     */
    void publish_trajectory_visualization(nb_arm_xdof &arm_model);

    // motor_driver& get_motor_sim();
    rclcpp::Node::SharedPtr node_;
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr trajectory_pub_;
    // --- 导纳控制状态变量 ---
    // 这些变量存储了导纳控制器的内部状态，并在每个控制周期中更新。

    /**
     * @brief 导纳位置偏移量 (q_adm)。
     * 这是由外力通过导纳模型积分产生的位置偏移。
     * 最终的目标位置将是 q_ref + q_adm。
     */
    KDL::JntArray admittance_pos_offset_jnt_;

    /**
     * @brief 导纳速度偏移量 (q_dot_adm)。
     * 这是由外力通过导纳模型积分产生的速度偏移。
     * 最终的目标速度将是 q_dot_ref + q_dot_adm。
     */
    KDL::JntArray admittance_vel_offset_jnt_;

    /**
     * @brief 用于欧拉积分的临时位置变量。
     * 在计算 admittance_pos_offset_jnt_ 时用作中间存储，以避免KDL函数中的混叠问题。
     */
    KDL::JntArray admittance_pos_offset_jnt_temp;

    /**
     * @brief 用于欧拉积分的临时速度变量。
     * 在计算 admittance_vel_offset_jnt_ 时用作中间存储。
     */
    KDL::JntArray admittance_vel_offset_jnt_temp;

    /**
     * @brief 经过低通滤波后的外部力矩 (τ_ext_filtered)。
     * 这是从原始估算力矩中滤除噪声后得到的平滑信号，作为导纳模型的输入。
     */
    KDL::JntArray tor_ext_filtered_;

    // 存储外部力矩零点偏置
    KDL::JntArray tor_ext_bias;
    /**
     * @brief 在开始运动前调用，用于标定外部力矩估计的零点偏置
     */
    void calibrate_external_force(nb_arm_xdof &arm_model, const KDL::JntArray &current_motor_torques);

    // 接触检测接口
    /**
     * @brief 设置接触检测参数
     * @param enable 是否启用接触检测
     * @param threshold_z Z轴上的力阈值 (单位: N)。
     * 假设Z轴是接近方向, 当 |Fz| > threshold_z 时触发。
     */
    void set_touch_detection(bool enable, float threshold_z);
    /**
     * @brief 检查是否已检测到稳定接触
     * @return bool
     */
    bool is_touch_detected() const;
    /**
     * @brief (私有) 估计末端执行器（EEF）的力/力矩
     * @param arm_model 机械臂模型，用于获取雅可比
     */
    void estimate_eef_wrench_frame(nb_arm_xdof &arm_model);
    /**
     * @brief 是否启用接触检测的标志
     */
    /**
     * @brief 存储末端6D力/力矩 (fx, fy, fz, tx, ty, tz)
     */
    KDL::Wrench f_ext_eef;
    bool enable_touch_detection = false;

    /**
     * @brief Z轴接触力阈值 (单位: N)
     */
    float touch_force_threshold_z_;

    /**
     * @brief 接触检测标志 (一旦触发, 保持为 true)
     */
    bool touch_detected = false;

    // Trajectory related
    std::vector<KDL::JntArray> q_ref_trajectory_;
    std::vector<KDL::JntArray> qdot_ref_trajectory_;
    std::vector<KDL::JntArray> qddot_ref_trajectory_;
    std::unique_ptr<KDL::Trajectory_Composite> trajectory_com;
    // Control-related members
    nb_arm_xdof arm_model_;
    trajectory_generator traj_gen_;
    std::unique_ptr<KDL::Path> path_;
    trajectory_generator traj_generator_;

private:
    // ROS
    // std::unique_ptr<KDL::Trajectory_Composite> trajectory_com;
    // std::vector<KDL::JntArray> q_ref_trajectory_;
    // std::vector<KDL::JntArray> qdot_ref_trajectory_;
    // std::vector<KDL::JntArray> qddot_ref_trajectory_;
    // 新增: 用于状态驱动跟踪的起始搜索索引
    size_t last_closest_index_ = 0;
    // 控制频率
    const float control_frequency = 100.0;
};

#endif // ARM_CONTROL_HPP
