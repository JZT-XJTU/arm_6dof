// NOLINT: This file starts with a BOM since it contain non-ASCII characters
// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arm_msgs:msg/MotorMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_H_
#define ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/MotorMitCmdTau in the package arm_msgs.
typedef struct arm_msgs__msg__MotorMitCmdTau
{
  /// motor target torque(有mit控制模式公式计算得到)
  float tau;
} arm_msgs__msg__MotorMitCmdTau;

// Struct for a sequence of arm_msgs__msg__MotorMitCmdTau.
typedef struct arm_msgs__msg__MotorMitCmdTau__Sequence
{
  arm_msgs__msg__MotorMitCmdTau * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arm_msgs__msg__MotorMitCmdTau__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_H_
