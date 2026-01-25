cmake_minimum_required(VERSION 3.8)
project(nb_arm)

# Set C++ Standard to C++20 to support designated initializers
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# 查找 libserial 库
find_library(
    LIBSERIAL_LIBRARY 
    NAMES serial 
    PATHS /usr/lib /usr/lib/x86_64-linux-gnu 
    NO_DEFAULT_PATH
    REQUIRED)
find_path(LIBSERIAL_INCLUDE_DIR NAMES libserial/SerialPort.h PATHS /usr/include REQUIRED)
# 打印调试信息，确保路径被正确设置
message(STATUS "ts LIBSERIAL_LIBRARY: ${LIBSERIAL_LIBRARY}")
message(STATUS "ts LIBSERIAL_INCLUDE_DIR: ${LIBSERIAL_INCLUDE_DIR}")

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(orocos_kdl REQUIRED)
find_package(kdl_parser REQUIRED)
find_package(urdf REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(arm_msgs REQUIRED)
find_package(Threads REQUIRED)
# 查找 libusb-1.0 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# 编译封装DM_MOTOR库
add_library(motor STATIC
    src/protocol/damiao.cpp 
)
# 添加DM_MOTOR库头文件搜索路径
target_include_directories(motor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LIBUSB_INCLUDE_DIRS}     # 添加 libusb 头文件路径
)
# motor库链接libu2canfd.a，libu2canfd.a需要链接线程和libusb
target_link_libraries(motor PRIVATE
    Threads::Threads
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libu2canfd.a
    ${LIBUSB_LIBRARIES}
)
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(arm_main_node 
src/nodes/arm_main_node.cpp
# src/drivers/motor_driver.cpp
src/mathematical_model/nb_arm_xdof.cpp
# src/controllers/arm_control.cpp
# src/controllers/arm_control_sim.cpp
src/protocol/damiao.cpp
src/controllers/controller.cpp
src/path/trajectory_generator.cpp
)

target_link_libraries(arm_main_node PRIVATE
    Eigen3::Eigen 
    ${LIBSERIAL_LIBRARY} 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libu2canfd.a
    ${LIBUSB_LIBRARIES}
)

add_executable(arm_control_sim_node
src/nodes/arm_control_sim_node.cpp
src/mathematical_model/nb_arm_xdof.cpp
src/controllers/arm_control.cpp
src/controllers/arm_control_sim.cpp
src/path/trajectory_generator.cpp
src/drivers/motor_driver.cpp
)


#为目标 arm_main_node 指定包含目录，这里指定了两个目录，一个是当前目录下的 include 目录，另一个是安装目录下的 include 目录
#${CMAKE_CURRENT_SOURCE_DIR} 是当前 CMakeLists.txt 文件所在的目录
target_include_directories(arm_main_node PUBLIC
  #BUILD_INTERFACE 表示这是在构建阶段使用的目录。
  #这个目录就是${CMAKE_CURRENT_SOURCE_DIR}/include
  #在引用头文件时，就需要使用 #include "nb_arm/xxx.h" 的方式
  #如果${CMAKE_CURRENT_SOURCE_DIR}/include/nb_arm，则只需要 #include "xxx.h" 的方式
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  
  #INSTALL_INTERFACE 表示这是在安装阶段使用的目录
  #这个目录是include
  $<INSTALL_INTERFACE:include>
  ${LIBSERIAL_INCLUDE_DIR}
  PRIVATE motor

  )

target_include_directories(arm_control_sim_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
  
#链接库
ament_target_dependencies(arm_main_node PUBLIC
    "rclcpp"
    "urdf"
    "sensor_msgs"
    "nav_msgs"
    "tf2_ros"
    "geometry_msgs"
    "trajectory_msgs"
    "std_msgs"
    "arm_msgs"
    "orocos_kdl"
    "kdl_parser"
)

target_link_libraries(arm_main_node PRIVATE
    Eigen3::Eigen 
    ${LIBSERIAL_LIBRARY} 
)

ament_target_dependencies(arm_control_sim_node PUBLIC
    "rclcpp"
    "urdf"
    "sensor_msgs"
    "nav_msgs"
    "tf2_ros"
    "geometry_msgs"
    "trajectory_msgs"
    "std_msgs"
    "arm_msgs"
    "orocos_kdl"
    "kdl_parser"
)

target_link_libraries(arm_control_sim_node PRIVATE
    Eigen3::Eigen 
)


install(TARGETS
  arm_main_node
  arm_control_sim_node
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY urdf
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY rviz
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
