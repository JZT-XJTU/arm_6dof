// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from arm_msgs:msg/MotorCommand.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__TRAITS_HPP_
#define ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "arm_msgs/msg/detail/motor_command__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

namespace arm_msgs
{

namespace msg
{

inline void to_flow_style_yaml(
  const MotorCommand & msg,
  std::ostream & out)
{
  out << "{";
  // member: q
  {
    out << "q: ";
    rosidl_generator_traits::value_to_yaml(msg.q, out);
    out << ", ";
  }

  // member: dq
  {
    out << "dq: ";
    rosidl_generator_traits::value_to_yaml(msg.dq, out);
    out << ", ";
  }

  // member: tau
  {
    out << "tau: ";
    rosidl_generator_traits::value_to_yaml(msg.tau, out);
    out << ", ";
  }

  // member: kp
  {
    out << "kp: ";
    rosidl_generator_traits::value_to_yaml(msg.kp, out);
    out << ", ";
  }

  // member: kd
  {
    out << "kd: ";
    rosidl_generator_traits::value_to_yaml(msg.kd, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const MotorCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: q
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "q: ";
    rosidl_generator_traits::value_to_yaml(msg.q, out);
    out << "\n";
  }

  // member: dq
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "dq: ";
    rosidl_generator_traits::value_to_yaml(msg.dq, out);
    out << "\n";
  }

  // member: tau
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "tau: ";
    rosidl_generator_traits::value_to_yaml(msg.tau, out);
    out << "\n";
  }

  // member: kp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "kp: ";
    rosidl_generator_traits::value_to_yaml(msg.kp, out);
    out << "\n";
  }

  // member: kd
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "kd: ";
    rosidl_generator_traits::value_to_yaml(msg.kd, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const MotorCommand & msg, bool use_flow_style = false)
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
  const arm_msgs::msg::MotorCommand & msg,
  std::ostream & out, size_t indentation = 0)
{
  arm_msgs::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use arm_msgs::msg::to_yaml() instead")]]
inline std::string to_yaml(const arm_msgs::msg::MotorCommand & msg)
{
  return arm_msgs::msg::to_yaml(msg);
}

template<>
inline const char * data_type<arm_msgs::msg::MotorCommand>()
{
  return "arm_msgs::msg::MotorCommand";
}

template<>
inline const char * name<arm_msgs::msg::MotorCommand>()
{
  return "arm_msgs/msg/MotorCommand";
}

template<>
struct has_fixed_size<arm_msgs::msg::MotorCommand>
  : std::integral_constant<bool, true> {};

template<>
struct has_bounded_size<arm_msgs::msg::MotorCommand>
  : std::integral_constant<bool, true> {};

template<>
struct is_message<arm_msgs::msg::MotorCommand>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__TRAITS_HPP_
