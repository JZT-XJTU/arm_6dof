// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arm_msgs:msg/MotorState.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_STATE__STRUCT_H_
#define ARM_MSGS__MSG__DETAIL__MOTOR_STATE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/MotorState in the package arm_msgs.
typedef struct arm_msgs__msg__MotorState
{
  /// motor current position (rad)
  float q;
  /// motor current speed (rad/s)
  float dq;
  /// motor current acceleration (rad/s)
  float ddq;
  /// current estimated output torque (N*m)
  float tau_est;
  /// current estimated output current (N*m)
  float cur;
} arm_msgs__msg__MotorState;

// Struct for a sequence of arm_msgs__msg__MotorState.
typedef struct arm_msgs__msg__MotorState__Sequence
{
  arm_msgs__msg__MotorState * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arm_msgs__msg__MotorState__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_STATE__STRUCT_H_
