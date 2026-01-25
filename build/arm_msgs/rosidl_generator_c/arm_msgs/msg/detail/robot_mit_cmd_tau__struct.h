// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arm_msgs:msg/RobotMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__STRUCT_H_
#define ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'cmd_tor'
#include "arm_msgs/msg/detail/motor_mit_cmd_tau__struct.h"

/// Struct defined in msg/RobotMitCmdTau in the package arm_msgs.
typedef struct arm_msgs__msg__RobotMitCmdTau
{
  arm_msgs__msg__MotorMitCmdTau__Sequence cmd_tor;
} arm_msgs__msg__RobotMitCmdTau;

// Struct for a sequence of arm_msgs__msg__RobotMitCmdTau.
typedef struct arm_msgs__msg__RobotMitCmdTau__Sequence
{
  arm_msgs__msg__RobotMitCmdTau * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arm_msgs__msg__RobotMitCmdTau__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__ROBOT_MIT_CMD_TAU__STRUCT_H_
