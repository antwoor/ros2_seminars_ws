from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen'
        ),
        Node(
            package='star',
            executable='qr_decoder',
            name='qr_decoder',
            output='screen'
        ),
        Node(
            package='star',
            executable='navigation',
            name='navigation',
            output='screen'
        ),
        Node(
            package='nav2_amcl',
            executable='amcl',
            name='amcl',
            output='screen',
            parameters=[{'use_sim_time': False}]
        ),
        Node(
            package='nav2_controller',
            executable='controller_server',
            name='controller_server',
            output='screen'
        ),
        Node(
            package='nav2_planner',
            executable='planner_server',
            name='planner_server',
            output='screen'
        ),
        # Node(
        #     package='nav2_recoveries',
        #     executable='recoveries_server',
        #     name='recoveries_server',
        #     output='screen'
        # ),
        Node(
            package='nav2_bt_navigator',
            executable='bt_navigator',
            name='bt_navigator',
            output='screen'
        ),
        Node(
            package='nav2_lifecycle_manager',
            executable='lifecycle_manager',
            name='lifecycle_manager_navigation',
            output='screen',
            parameters=[{'use_sim_time': False},
                        {'autostart': True},
                        {'node_names': ['amcl', 'controller_server', 'planner_server', 'recoveries_server', 'bt_navigator']}]
        ),
    ])
