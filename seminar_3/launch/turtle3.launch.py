from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    radius = '1'  #initial radius of spiral
    growth_rate = '1'  # koefficient of spiral growth
    SpiralTurtle = Node(
        package='seminar_3',
        executable='task1_spiral',
        name='archi_spiral',
        arguments=[radius, growth_rate],
        parameters=[{'cmd_vel_topic': '/cmd_vel'}]    
    )
    return LaunchDescription([
        SpiralTurtle
    ])