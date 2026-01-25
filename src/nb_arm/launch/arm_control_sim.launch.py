from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    urdf_file_name = 'ur5.urdf'
    urdf = os.path.join(
        get_package_share_directory('nb_arm'),
        'urdf',
        urdf_file_name)
    with open(urdf, 'r') as infp:
        robot_desc = infp.read()

    rviz_config_file = os.path.join(
        get_package_share_directory('nb_arm'),
        'rviz',
        'arm_sim.rviz')

    return LaunchDescription([
        # Node(
        #     package='robot_state_publisher',
        #     executable='robot_state_publisher',
        #     name='robot_state_publisher',
        #     output='screen',
        #     parameters=[{'robot_description': robot_desc}],
        #     arguments=[urdf]
        # ),
        Node(
            package='nb_arm',
            executable='arm_control_sim_node',
            name='arm_control_sim_node',
            output='screen'
        ),
        # Node(
        #     package='rviz2',
        #     executable='rviz2',
        #     name='rviz2',
        #     output='screen',
        #     arguments=['-d', rviz_config_file]
        # )
    ])
