#ifndef ARM_CONTROL_SIM_HPP_
#define ARM_CONTROL_SIM_HPP_

#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <mutex>
#include <vector>
#include <utility>

#include "std_msgs/msg/float64_multi_array.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "arm_msgs/msg/robot_command.hpp"
#include "nb_arm/mathematical_model/nb_arm_xdof.hpp"
#include "nb_arm/drivers/motor_driver.hpp"
#include "nb_arm/controllers/arm_control.hpp"
#include "nb_arm/path/trajectory_generator.hpp"


class ArmControlSim
{
public:
    explicit ArmControlSim(rclcpp::Node::SharedPtr node);
    ~ArmControlSim();

    void send_robot_command(std_msgs::msg::Float64MultiArray cmd_msg, const float tor_ff[MOTOR_NUM]);
    // void recv_state(std::vector<MotorState> &motor_states);

private:
    // ROS 回调函数
    void robot_callback(const sensor_msgs::msg::JointState::SharedPtr msg);

    // 线程中运行的控制循环
    void run_control();

    rclcpp::Node::SharedPtr node_;

    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr robot_cmd_pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr robot_state_sub_;
    std_msgs::msg::Float64MultiArray effort_cmd_msg;

    std::thread sim_thread_;
    std::mutex state_mutex_;
    std::mutex cmd_mutex_;
    bool is_running_ = true;

    // arm_msgs::msg::RobotCommand robot_cmd_;
    motor_driver motor_sim;
    std::vector<std::string> joint_names_;

    // Control-related members
    // nb_arm_xdof arm_model_;
    arm_control controller_;

    // std::unique_ptr<KDL::Trajectory_Composite> trajectory_com;
    // std::vector<KDL::JntArray> q_ref_trajectory_;
    // std::vector<KDL::JntArray> qdot_ref_trajectory_;
    // std::vector<KDL::JntArray> qddot_ref_trajectory_;
    // trajectory_generator traj_gen_;
    // std::unique_ptr<KDL::Path> path_;

    size_t last_closest_index_ = 0;

    const float control_frequency = 100.0;
};

#endif // ARM_CONTROL_SIM_HPP_
