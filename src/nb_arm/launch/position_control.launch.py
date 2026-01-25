from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    # 获取包的共享目录
    pkg_share = get_package_share_directory('nb_arm')
    
    # URDF 文件路径
    urdf_file = os.path.join(pkg_share, 'urdf', 'first_robot.urdf')

    # 节点配置
    arm_node = Node(
        package='nb_arm',
        executable='arm_main_node',
        name='arm_node',
        output='screen',
        parameters=[
            {'urdf_file': urdf_file},
            {'control_mode': 'Zero'}  # 设置控制模式为 Position
        ]
    )

    return LaunchDescription([
        arm_node
    ])
