// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from arm_msgs:msg/RobotState.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_
#define ARM_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "arm_msgs/msg/detail/robot_state__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace arm_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotState_motor_state
{
public:
  Init_RobotState_motor_state()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::arm_msgs::msg::RobotState motor_state(::arm_msgs::msg::RobotState::_motor_state_type arg)
  {
    msg_.motor_state = std::move(arg);
    return std::move(msg_);
  }

private:
  ::arm_msgs::msg::RobotState msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::arm_msgs::msg::RobotState>()
{
  return arm_msgs::msg::builder::Init_RobotState_motor_state();
}

}  // namespace arm_msgs

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_STATE__BUILDER_HPP_
