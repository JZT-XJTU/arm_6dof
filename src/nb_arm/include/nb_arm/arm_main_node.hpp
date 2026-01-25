#ifndef ARM_MAIN_NODE_HPP_
#define ARM_MAIN_NODE_HPP_

#include <rclcpp/rclcpp.hpp>
#include "nb_arm/path/trajectory_generator.hpp"
#include "nb_arm/mathematical_model/nb_arm_xdof.hpp"
#include "nb_arm/controllers/controller.hpp"
#include "nb_arm/protocol/damiao.h"
#include <sensor_msgs/msg/joint_state.hpp>

class arm_node : public rclcpp::Node
{
public:
    static arm_node *node_instance;
    std::shared_ptr<damiao::Motor_Control> motor;
    arm_node();
    ~arm_node();
    void init(); // Add init method
    static void stopArm()
    {
        std::cout << "\n >>>>>>失能机械臂" << std::endl;
        // 需要一个实例来调用 motorControl 的成员函数
        if (node_instance)
        {
            // node_instance->motorControl.DisableMotors(node_instance->serial);
            if (node_instance)
            {
                node_instance->motor->disable_motor();
            }
        }
    }

    static void signalHandler(int signum)
    {
        std::cout << "\n >>>>>>接收到SIGINT(" << signum << ")，准备终止程序。" << std::endl;
        // 需要一个实例来调用 motorControl 和 serial 的成员函数
        if (node_instance)
        {
            // node_instance->motorControl.DisableMotors(node_instance->serial);
            // node_instance->serial.stopReceiving();
            if (node_instance)
            {
                node_instance->motor->disable_motor();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(2000)); // 微秒
        }
        std::exit(signum); // 程序正常退出，将触发atexit的调用
    }

private:
    nb_arm_xdof arm_model;
    controller arm_controller;
    State mode;
    rclcpp::TimerBase::SharedPtr timer_;
};

#endif // ARM_MAIN_NODE_HPP_
