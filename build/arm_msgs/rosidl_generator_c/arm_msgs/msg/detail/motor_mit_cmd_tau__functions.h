// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from arm_msgs:msg/MotorMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__FUNCTIONS_H_
#define ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "arm_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "arm_msgs/msg/detail/motor_mit_cmd_tau__struct.h"

/// Initialize msg/MotorMitCmdTau message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * arm_msgs__msg__MotorMitCmdTau
 * )) before or use
 * arm_msgs__msg__MotorMitCmdTau__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__init(arm_msgs__msg__MotorMitCmdTau * msg);

/// Finalize msg/MotorMitCmdTau message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
void
arm_msgs__msg__MotorMitCmdTau__fini(arm_msgs__msg__MotorMitCmdTau * msg);

/// Create msg/MotorMitCmdTau message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * arm_msgs__msg__MotorMitCmdTau__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
arm_msgs__msg__MotorMitCmdTau *
arm_msgs__msg__MotorMitCmdTau__create();

/// Destroy msg/MotorMitCmdTau message.
/**
 * It calls
 * arm_msgs__msg__MotorMitCmdTau__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
void
arm_msgs__msg__MotorMitCmdTau__destroy(arm_msgs__msg__MotorMitCmdTau * msg);

/// Check for msg/MotorMitCmdTau message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__are_equal(const arm_msgs__msg__MotorMitCmdTau * lhs, const arm_msgs__msg__MotorMitCmdTau * rhs);

/// Copy a msg/MotorMitCmdTau message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__copy(
  const arm_msgs__msg__MotorMitCmdTau * input,
  arm_msgs__msg__MotorMitCmdTau * output);

/// Initialize array of msg/MotorMitCmdTau messages.
/**
 * It allocates the memory for the number of elements and calls
 * arm_msgs__msg__MotorMitCmdTau__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__Sequence__init(arm_msgs__msg__MotorMitCmdTau__Sequence * array, size_t size);

/// Finalize array of msg/MotorMitCmdTau messages.
/**
 * It calls
 * arm_msgs__msg__MotorMitCmdTau__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
void
arm_msgs__msg__MotorMitCmdTau__Sequence__fini(arm_msgs__msg__MotorMitCmdTau__Sequence * array);

/// Create array of msg/MotorMitCmdTau messages.
/**
 * It allocates the memory for the array and calls
 * arm_msgs__msg__MotorMitCmdTau__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
arm_msgs__msg__MotorMitCmdTau__Sequence *
arm_msgs__msg__MotorMitCmdTau__Sequence__create(size_t size);

/// Destroy array of msg/MotorMitCmdTau messages.
/**
 * It calls
 * arm_msgs__msg__MotorMitCmdTau__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
void
arm_msgs__msg__MotorMitCmdTau__Sequence__destroy(arm_msgs__msg__MotorMitCmdTau__Sequence * array);

/// Check for msg/MotorMitCmdTau message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__Sequence__are_equal(const arm_msgs__msg__MotorMitCmdTau__Sequence * lhs, const arm_msgs__msg__MotorMitCmdTau__Sequence * rhs);

/// Copy an array of msg/MotorMitCmdTau messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_arm_msgs
bool
arm_msgs__msg__MotorMitCmdTau__Sequence__copy(
  const arm_msgs__msg__MotorMitCmdTau__Sequence * input,
  arm_msgs__msg__MotorMitCmdTau__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__FUNCTIONS_H_
