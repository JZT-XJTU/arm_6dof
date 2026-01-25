// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from arm_msgs:msg/MotorCommand.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__STRUCT_H_
#define ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in msg/MotorCommand in the package arm_msgs.
typedef struct arm_msgs__msg__MotorCommand
{
  /// motor target position
  float q;
  /// motor target velocity
  float dq;
  /// motor target torque
  float tau;
  /// motor spring stiffness coefficient
  float kp;
  /// motor damper coefficient
  float kd;
} arm_msgs__msg__MotorCommand;

// Struct for a sequence of arm_msgs__msg__MotorCommand.
typedef struct arm_msgs__msg__MotorCommand__Sequence
{
  arm_msgs__msg__MotorCommand * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} arm_msgs__msg__MotorCommand__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_COMMAND__STRUCT_H_
