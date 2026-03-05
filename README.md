# arm_6dof


这是一个基于 ROS 2 和 KDL 的机械臂控制项目, 项目运行依赖下位机和 DM USB2FDCAN 通信模块   
下位机代码仓库：

## 📦 项目结构

工作空间包含以下核心 ROS 2 功能包：

- **`nb_arm`**: 机械臂的核心控制功能包
  - 使用了 `rclcpp` 编写 C++ 节点
  - 集成了 `Eigen3`、`orocos_kdl` 和 `kdl_parser` 进行动力学/运动学计算
  - 支持 `urdf` 解析
  - 依赖标准 ROS 2 消息（`sensor_msgs`, `nav_msgs`, `tf2_ros` 等）和自定义消息 `arm_msgs`
- **`arm_msgs`**: 自定义通信消息包。定义了底层电机和直接关节控制的数据结构（ MIT 控制模式）。主要包含：
  - `MotorCommand.msg` / `MotorState.msg`
  - `MotorMitCmdTau.msg`
  - `RobotCommand.msg` / `RobotState.msg`
  - `RobotMitCmdTau.msg`

## 🛠️ 环境依赖

确保系统已经安装了 ROS 2 (如 Humble 或 Foxy 等版本) 及相关的编译工具（`colcon` 等）。项目中强依赖如下库，请确保系统已经安装：

```bash
# ROS2 常见依赖
sudo apt install ros-<distro>-sensor-msgs ros-<distro>-nav-msgs ros-<distro>-tf2-ros ros-<distro>-xacro

# 运动学及数学库依赖
sudo apt install ros-<distro>-orocos-kdl ros-<distro>-kdl-parser
sudo apt install libeigen3-dev
```
*(注：需要将 `<distro>` 替换为具体 ROS 2 版本名)*

## 🚀 编译与构建

克隆项目并进入工作空间根目录（`~/arm_ws`），然后使用 `colcon` 进行构建

## ⚙️ 运行与测试

在每次新打开一个终端或编译完成后，需要 source 环境：

```bash
source install/setup.bash
```

可以启动对应的 launch 文件执行对应功能，例如运行position_control

```bash
ros2 launch nb_arm position_control.launch.py
```

在运行之前需要使用 DM USB2FDCAN 模块与下位机的 FDCAN 外设连接之后并使用对应下位机的代码，如此才能正常驱动机械臂电机

