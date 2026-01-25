// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from arm_msgs:msg/RobotCommand.idl
// generated code does not contain a copyright notice
#include "arm_msgs/msg/detail/robot_command__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `motor_command`
#include "arm_msgs/msg/detail/motor_command__functions.h"

bool
arm_msgs__msg__RobotCommand__init(arm_msgs__msg__RobotCommand * msg)
{
  if (!msg) {
    return false;
  }
  // motor_command
  if (!arm_msgs__msg__MotorCommand__Sequence__init(&msg->motor_command, 0)) {
    arm_msgs__msg__RobotCommand__fini(msg);
    return false;
  }
  return true;
}

void
arm_msgs__msg__RobotCommand__fini(arm_msgs__msg__RobotCommand * msg)
{
  if (!msg) {
    return;
  }
  // motor_command
  arm_msgs__msg__MotorCommand__Sequence__fini(&msg->motor_command);
}

bool
arm_msgs__msg__RobotCommand__are_equal(const arm_msgs__msg__RobotCommand * lhs, const arm_msgs__msg__RobotCommand * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // motor_command
  if (!arm_msgs__msg__MotorCommand__Sequence__are_equal(
      &(lhs->motor_command), &(rhs->motor_command)))
  {
    return false;
  }
  return true;
}

bool
arm_msgs__msg__RobotCommand__copy(
  const arm_msgs__msg__RobotCommand * input,
  arm_msgs__msg__RobotCommand * output)
{
  if (!input || !output) {
    return false;
  }
  // motor_command
  if (!arm_msgs__msg__MotorCommand__Sequence__copy(
      &(input->motor_command), &(output->motor_command)))
  {
    return false;
  }
  return true;
}

arm_msgs__msg__RobotCommand *
arm_msgs__msg__RobotCommand__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  arm_msgs__msg__RobotCommand * msg = (arm_msgs__msg__RobotCommand *)allocator.allocate(sizeof(arm_msgs__msg__RobotCommand), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(arm_msgs__msg__RobotCommand));
  bool success = arm_msgs__msg__RobotCommand__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
arm_msgs__msg__RobotCommand__destroy(arm_msgs__msg__RobotCommand * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    arm_msgs__msg__RobotCommand__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
arm_msgs__msg__RobotCommand__Sequence__init(arm_msgs__msg__RobotCommand__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  arm_msgs__msg__RobotCommand * data = NULL;

  if (size) {
    data = (arm_msgs__msg__RobotCommand *)allocator.zero_allocate(size, sizeof(arm_msgs__msg__RobotCommand), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = arm_msgs__msg__RobotCommand__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        arm_msgs__msg__RobotCommand__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
arm_msgs__msg__RobotCommand__Sequence__fini(arm_msgs__msg__RobotCommand__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      arm_msgs__msg__RobotCommand__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

arm_msgs__msg__RobotCommand__Sequence *
arm_msgs__msg__RobotCommand__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  arm_msgs__msg__RobotCommand__Sequence * array = (arm_msgs__msg__RobotCommand__Sequence *)allocator.allocate(sizeof(arm_msgs__msg__RobotCommand__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = arm_msgs__msg__RobotCommand__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
arm_msgs__msg__RobotCommand__Sequence__destroy(arm_msgs__msg__RobotCommand__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    arm_msgs__msg__RobotCommand__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
arm_msgs__msg__RobotCommand__Sequence__are_equal(const arm_msgs__msg__RobotCommand__Sequence * lhs, const arm_msgs__msg__RobotCommand__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!arm_msgs__msg__RobotCommand__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
arm_msgs__msg__RobotCommand__Sequence__copy(
  const arm_msgs__msg__RobotCommand__Sequence * input,
  arm_msgs__msg__RobotCommand__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(arm_msgs__msg__RobotCommand);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    arm_msgs__msg__RobotCommand * data =
      (arm_msgs__msg__RobotCommand *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!arm_msgs__msg__RobotCommand__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          arm_msgs__msg__RobotCommand__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!arm_msgs__msg__RobotCommand__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
