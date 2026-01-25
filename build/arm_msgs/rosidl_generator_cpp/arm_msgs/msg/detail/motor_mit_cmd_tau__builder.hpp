// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from arm_msgs:msg/MotorMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__BUILDER_HPP_
#define ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "arm_msgs/msg/detail/motor_mit_cmd_tau__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace arm_msgs
{

namespace msg
{

namespace builder
{

class Init_MotorMitCmdTau_tau
{
public:
  Init_MotorMitCmdTau_tau()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::arm_msgs::msg::MotorMitCmdTau tau(::arm_msgs::msg::MotorMitCmdTau::_tau_type arg)
  {
    msg_.tau = std::move(arg);
    return std::move(msg_);
  }

private:
  ::arm_msgs::msg::MotorMitCmdTau msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::arm_msgs::msg::MotorMitCmdTau>()
{
  return arm_msgs::msg::builder::Init_MotorMitCmdTau_tau();
}

}  // namespace arm_msgs

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__BUILDER_HPP_
