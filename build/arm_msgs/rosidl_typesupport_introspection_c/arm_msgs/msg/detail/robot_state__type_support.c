// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from arm_msgs:msg/RobotState.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "arm_msgs/msg/detail/robot_state__rosidl_typesupport_introspection_c.h"
#include "arm_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "arm_msgs/msg/detail/robot_state__functions.h"
#include "arm_msgs/msg/detail/robot_state__struct.h"


// Include directives for member types
// Member `motor_state`
#include "arm_msgs/msg/motor_state.h"
// Member `motor_state`
#include "arm_msgs/msg/detail/motor_state__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  arm_msgs__msg__RobotState__init(message_memory);
}

void arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_fini_function(void * message_memory)
{
  arm_msgs__msg__RobotState__fini(message_memory);
}

size_t arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__size_function__RobotState__motor_state(
  const void * untyped_member)
{
  const arm_msgs__msg__MotorState__Sequence * member =
    (const arm_msgs__msg__MotorState__Sequence *)(untyped_member);
  return member->size;
}

const void * arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_const_function__RobotState__motor_state(
  const void * untyped_member, size_t index)
{
  const arm_msgs__msg__MotorState__Sequence * member =
    (const arm_msgs__msg__MotorState__Sequence *)(untyped_member);
  return &member->data[index];
}

void * arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_function__RobotState__motor_state(
  void * untyped_member, size_t index)
{
  arm_msgs__msg__MotorState__Sequence * member =
    (arm_msgs__msg__MotorState__Sequence *)(untyped_member);
  return &member->data[index];
}

void arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__fetch_function__RobotState__motor_state(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const arm_msgs__msg__MotorState * item =
    ((const arm_msgs__msg__MotorState *)
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_const_function__RobotState__motor_state(untyped_member, index));
  arm_msgs__msg__MotorState * value =
    (arm_msgs__msg__MotorState *)(untyped_value);
  *value = *item;
}

void arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__assign_function__RobotState__motor_state(
  void * untyped_member, size_t index, const void * untyped_value)
{
  arm_msgs__msg__MotorState * item =
    ((arm_msgs__msg__MotorState *)
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_function__RobotState__motor_state(untyped_member, index));
  const arm_msgs__msg__MotorState * value =
    (const arm_msgs__msg__MotorState *)(untyped_value);
  *item = *value;
}

bool arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__resize_function__RobotState__motor_state(
  void * untyped_member, size_t size)
{
  arm_msgs__msg__MotorState__Sequence * member =
    (arm_msgs__msg__MotorState__Sequence *)(untyped_member);
  arm_msgs__msg__MotorState__Sequence__fini(member);
  return arm_msgs__msg__MotorState__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_member_array[1] = {
  {
    "motor_state",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(arm_msgs__msg__RobotState, motor_state),  // bytes offset in struct
    NULL,  // default value
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__size_function__RobotState__motor_state,  // size() function pointer
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_const_function__RobotState__motor_state,  // get_const(index) function pointer
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__get_function__RobotState__motor_state,  // get(index) function pointer
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__fetch_function__RobotState__motor_state,  // fetch(index, &value) function pointer
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__assign_function__RobotState__motor_state,  // assign(index, value) function pointer
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__resize_function__RobotState__motor_state  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_members = {
  "arm_msgs__msg",  // message namespace
  "RobotState",  // message name
  1,  // number of fields
  sizeof(arm_msgs__msg__RobotState),
  arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_member_array,  // message members
  arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_init_function,  // function to initialize message memory (memory has to be allocated)
  arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_type_support_handle = {
  0,
  &arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_arm_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, arm_msgs, msg, RobotState)() {
  arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, arm_msgs, msg, MotorState)();
  if (!arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_type_support_handle.typesupport_identifier) {
    arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &arm_msgs__msg__RobotState__rosidl_typesupport_introspection_c__RobotState_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
