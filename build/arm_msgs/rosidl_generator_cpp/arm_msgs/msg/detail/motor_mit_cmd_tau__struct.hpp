// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from arm_msgs:msg/MotorMitCmdTau.idl
// generated code does not contain a copyright notice

#ifndef ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_HPP_
#define ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__arm_msgs__msg__MotorMitCmdTau __attribute__((deprecated))
#else
# define DEPRECATED__arm_msgs__msg__MotorMitCmdTau __declspec(deprecated)
#endif

namespace arm_msgs
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct MotorMitCmdTau_
{
  using Type = MotorMitCmdTau_<ContainerAllocator>;

  explicit MotorMitCmdTau_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->tau = 0.0f;
    }
  }

  explicit MotorMitCmdTau_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->tau = 0.0f;
    }
  }

  // field types and members
  using _tau_type =
    float;
  _tau_type tau;

  // setters for named parameter idiom
  Type & set__tau(
    const float & _arg)
  {
    this->tau = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> *;
  using ConstRawPtr =
    const arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__arm_msgs__msg__MotorMitCmdTau
    std::shared_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__arm_msgs__msg__MotorMitCmdTau
    std::shared_ptr<arm_msgs::msg::MotorMitCmdTau_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const MotorMitCmdTau_ & other) const
  {
    if (this->tau != other.tau) {
      return false;
    }
    return true;
  }
  bool operator!=(const MotorMitCmdTau_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct MotorMitCmdTau_

// alias to use template instance with default allocator
using MotorMitCmdTau =
  arm_msgs::msg::MotorMitCmdTau_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace arm_msgs

#endif  // ARM_MSGS__MSG__DETAIL__MOTOR_MIT_CMD_TAU__STRUCT_HPP_
