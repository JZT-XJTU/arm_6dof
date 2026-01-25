// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arm_msgs:msg/RobotState.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_
#define ARM_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'motor_state'
#include "arm_msgs/msg/detail/motor_state__struct.h"

/// Struct defined in msg/RobotState in the package arm_msgs.
typedef struct arm_msgs__msg__RobotState
{
  arm_msgs__msg__MotorState__Sequence motor_state;
} arm_msgs__msg__RobotState;

// Struct for a sequence of arm_msgs__msg__RobotState.
typedef struct arm_msgs__msg__RobotState__Sequence
{
  arm_msgs__msg__RobotState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arm_msgs__msg__RobotState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_STATE__STRUCT_H_
