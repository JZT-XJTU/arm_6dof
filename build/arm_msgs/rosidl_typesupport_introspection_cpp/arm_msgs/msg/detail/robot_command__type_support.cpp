// generated from rosidl_typesupport_introspection_cpp/resource/idl__type_support.cpp.em
// with input from arm_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice

#include "array"
#include "cstddef"
#include "string"
#include "vector"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_interface/macros.h"
#include "arm_msgs/msg/detail/robot_command__struct.hpp"
#include "rosidl_typesupport_introspection_cpp/field_types.hpp"
#include "rosidl_typesupport_introspection_cpp/identifier.hpp"
#include "rosidl_typesupport_introspection_cpp/message_introspection.hpp"
#include "rosidl_typesupport_introspection_cpp/message_type_support_decl.hpp"
#include "rosidl_typesupport_introspection_cpp/visibility_control.h"

namespace arm_msgs
{

namespace msg
{

namespace rosidl_typesupport_introspection_cpp
{

void RobotCommand_init_function(
  void * message_memory, rosidl_runtime_cpp::MessageInitialization _init)
{
  new (message_memory) arm_msgs::msg::RobotCommand(_init);
}

void RobotCommand_fini_function(void * message_memory)
{
  auto typed_message = static_cast<arm_msgs::msg::RobotCommand *>(message_memory);
  typed_message->~RobotCommand();
}

size_t size_function__RobotCommand__motor_command(const void * untyped_member)
{
  const auto * member = reinterpret_cast<const std::vector<arm_msgs::msg::MotorCommand> *>(untyped_member);
  return member->size();
}

const void * get_const_function__RobotCommand__motor_command(const void * untyped_member, size_t index)
{
  const auto & member =
    *reinterpret_cast<const std::vector<arm_msgs::msg::MotorCommand> *>(untyped_member);
  return &member[index];
}

void * get_function__RobotCommand__motor_command(void * untyped_member, size_t index)
{
  auto & member =
    *reinterpret_cast<std::vector<arm_msgs::msg::MotorCommand> *>(untyped_member);
  return &member[index];
}

void fetch_function__RobotCommand__motor_command(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const auto & item = *reinterpret_cast<const arm_msgs::msg::MotorCommand *>(
    get_const_function__RobotCommand__motor_command(untyped_member, index));
  auto & value = *reinterpret_cast<arm_msgs::msg::MotorCommand *>(untyped_value);
  value = item;
}

void assign_function__RobotCommand__motor_command(
  void * untyped_member, size_t index, const void * untyped_value)
{
  auto & item = *reinterpret_cast<arm_msgs::msg::MotorCommand *>(
    get_function__RobotCommand__motor_command(untyped_member, index));
  const auto & value = *reinterpret_cast<const arm_msgs::msg::MotorCommand *>(untyped_value);
  item = value;
}

void resize_function__RobotCommand__motor_command(void * untyped_member, size_t size)
{
  auto * member =
    reinterpret_cast<std::vector<arm_msgs::msg::MotorCommand> *>(untyped_member);
  member->resize(size);
}

static const ::rosidl_typesupport_introspection_cpp::MessageMember RobotCommand_message_member_array[1] = {
  {
    "motor_command",  // name
    ::rosidl_typesupport_introspection_cpp::ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    ::rosidl_typesupport_introspection_cpp::get_message_type_support_handle<arm_msgs::msg::MotorCommand>(),  // members of sub message
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(arm_msgs::msg::RobotCommand, motor_command),  // bytes offset in struct
    nullptr,  // default value
    size_function__RobotCommand__motor_command,  // size() function pointer
    get_const_function__RobotCommand__motor_command,  // get_const(index) function pointer
    get_function__RobotCommand__motor_command,  // get(index) function pointer
    fetch_function__RobotCommand__motor_command,  // fetch(index, &value) function pointer
    assign_function__RobotCommand__motor_command,  // assign(index, value) function pointer
    resize_function__RobotCommand__motor_command  // resize(index) function pointer
  }
};

static const ::rosidl_typesupport_introspection_cpp::MessageMembers RobotCommand_message_members = {
  "arm_msgs::msg",  // message namespace
  "RobotCommand",  // message name
  1,  // number of fields
  sizeof(arm_msgs::msg::RobotCommand),
  RobotCommand_message_member_array,  // message members
  RobotCommand_init_function,  // function to initialize message memory (memory has to be allocated)
  RobotCommand_fini_function  // function to terminate message instance (will not free memory)
};

static const rosidl_message_type_support_t RobotCommand_message_type_support_handle = {
  ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  &RobotCommand_message_members,
  get_message_typesupport_handle_function,
};

}  // namespace rosidl_typesupport_introspection_cpp

}  // namespace msg

}  // namespace arm_msgs


namespace rosidl_typesupport_introspection_cpp
{

template<>
ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<arm_msgs::msg::RobotCommand>()
{
  return &::arm_msgs::msg::rosidl_typesupport_introspection_cpp::RobotCommand_message_type_support_handle;
}

}  // namespace rosidl_typesupport_introspection_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, arm_msgs, msg, RobotCommand)() {
  return &::arm_msgs::msg::rosidl_typesupport_introspection_cpp::RobotCommand_message_type_support_handle;
}

#ifdef __cplusplus
}
#endif
