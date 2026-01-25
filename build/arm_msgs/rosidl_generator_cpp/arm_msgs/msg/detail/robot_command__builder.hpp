// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from arm_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_
#define ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "arm_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace arm_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotCommand_motor_command
{
public:
  Init_RobotCommand_motor_command()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::arm_msgs::msg::RobotCommand motor_command(::arm_msgs::msg::RobotCommand::_motor_command_type arg)
  {
    msg_.motor_command = std::move(arg);
    return std::move(msg_);
  }

private:
  ::arm_msgs::msg::RobotCommand msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::arm_msgs::msg::RobotCommand>()
{
  return arm_msgs::msg::builder::Init_RobotCommand_motor_command();
}

}  // namespace arm_msgs

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__BUILDER_HPP_
