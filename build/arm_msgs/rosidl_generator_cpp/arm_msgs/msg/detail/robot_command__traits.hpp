// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from arm_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_
#define ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "arm_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'motor_command'
#include "arm_msgs/msg/detail/motor_command__traits.hpp"

namespace arm_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const RobotCommand & msg,
  std::ostream & out)
{
  out << "{";
  // member: motor_command
  {
    if (msg.motor_command.size() == 0) {
      out << "motor_command: []";
    } else {
      out << "motor_command: [";
      size_t pending_items = msg.motor_command.size();
      for (auto item : msg.motor_command) {
        to_flow_style_yaml(item, out);
        if (--pending_items > 0) {
          out << ", ";
        }
      }
      out << "]";
    }
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const RobotCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: motor_command
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    if (msg.motor_command.size() == 0) {
      out << "motor_command: []\n";
    } else {
      out << "motor_command:\n";
      for (auto item : msg.motor_command) {
        if (indentation > 0) {
          out << std::string(indentation, ' ');
        }
        out << "-\n";
        to_block_style_yaml(item, out, indentation + 2);
      }
    }
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const RobotCommand & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace arm_msgs

namespace rosidl_generator_traits
{

[[deprecated("use arm_msgs::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const arm_msgs::msg::RobotCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  arm_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use arm_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const arm_msgs::msg::RobotCommand & msg)
{
  return arm_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<arm_msgs::msg::RobotCommand>()
{
  return "arm_msgs::msg::RobotCommand";
}

template<>
inline const char * name<arm_msgs::msg::RobotCommand>()
{
  return "arm_msgs/msg/RobotCommand";
}

template<>
struct has_fixed_size<arm_msgs::msg::RobotCommand>
  : std::integral_constant<bool, false> {};

template<>
struct has_bounded_size<arm_msgs::msg::RobotCommand>
  : std::integral_constant<bool, false> {};

template<>
struct is_message<arm_msgs::msg::RobotCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_COMMAND__TRAITS_HPP_
