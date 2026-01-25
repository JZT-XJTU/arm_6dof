// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from arm_msgs:msg/RobotMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__BUILDER_HPP_
#define ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "arm_msgs/msg/detail/robot_mit_cmd_tau__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace arm_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotMitCmdTau_cmd_tor
{
public:
  Init_RobotMitCmdTau_cmd_tor()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::arm_msgs::msg::RobotMitCmdTau cmd_tor(::arm_msgs::msg::RobotMitCmdTau::_cmd_tor_type arg)
  {
    msg_.cmd_tor = std::move(arg);
    return std::move(msg_);
  }

private:
  ::arm_msgs::msg::RobotMitCmdTau msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::arm_msgs::msg::RobotMitCmdTau>()
{
  return arm_msgs::msg::builder::Init_RobotMitCmdTau_cmd_tor();
}

}  // namespace arm_msgs

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__BUILDER_HPP_
