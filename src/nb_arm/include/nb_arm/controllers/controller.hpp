#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>
#include "nb_arm/mathematical_model/nb_arm_xdof.hpp"
// #include "nb_arm/drivers/motor_driver.hpp"
#include "nb_arm/protocol/damiao.h"
#include "nb_arm/path/trajectory_generator.hpp"

enum class State
{
    HybridForce, // 力位混合控制
    Position,    // 位置控制（补偿前馈力矩）
    Zero,        // 设置零点
};

class controller
{
public:
    controller();
    ~controller() = default;
    void modeSwitch(State newState);
    void run(nb_arm_xdof &arm_model, damiao::Motor_Control &motor);
    void HybridForceControl(nb_arm_xdof &arm_model, damiao::Motor_Control &motor);
    void PositionControl(nb_arm_xdof &arm_model, damiao::Motor_Control &motor);
    void go_home(nb_arm_xdof &arm_model, damiao::Motor_Control &motor);

    // 可以存储一些固定点位便于生成预设路径

    bool get_trajectory(
        nb_arm_xdof &arm_model,
        trajectory_generator &traj_gen,
        std::unique_ptr<KDL::Path> path,
        bool is_trap,
        float spline_duration);

    State currentState;

    // Trajectory related
    std::vector<KDL::JntArray> q_ref_trajectory_;
    std::vector<KDL::JntArray> qdot_ref_trajectory_;
    std::vector<KDL::JntArray> qddot_ref_trajectory_;
    std::vector<KDL::Frame> x_ref_trajectory_;
    std::vector<KDL::Twist> xdot_ref_trajectory_;
    std::unique_ptr<KDL::Trajectory_Composite> trajectory_com;
    trajectory_generator traj_gen;

    // Position Control related
    size_t last_closest_index_ = 0;
    float distanceSq(const KDL::JntArray &a, const KDL::JntArray &b);

private:
    float control_frequency; // 控制频率
    std::mutex data_mutex;
    bool initialized = false;
};

#endif // CONTROLLER_HPP
