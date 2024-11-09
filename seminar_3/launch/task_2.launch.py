from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    TurtleSim = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='Main_turtle'
    )

    spawn_turtle2 = ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', "\"{x: 1.0, y: 1.0, theta: 1.57, name: 'turtle2'}\""],
        name='spawn_turtle2',
        shell = True
    )

    spawn_turtle3 = ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', "\"{x: 2.0, y: 2.0, theta: 1.57, name: 'turtle3'}\""],
        name='spawn_turtle3',
        shell = True
    )

    spawn_turtle4 = ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', "\"{x: 3.0, y: 3.0, theta: 1.57, name: 'turtle4'}\""],
        name='spawn_turtle4',
        shell = True
    )

    spawn_turtle5 = ExecuteProcess(
        cmd=['ros2', 'service', 'call', '/spawn', 'turtlesim/srv/Spawn', "\"{x: 4.0, y: 4.0, theta: 1.57, name: 'turtle5'}\""],
        name='spawn_turtle5',
        shell = True
    )
    radius = '1'  #initial radius of spiral
    growth_rate = '1'  # koefficient of spiral growth

    SpiralTurtle1 = Node(
        package='seminar_3',
        executable='task1_spiral',
        name='spiral1',
        arguments=[radius, growth_rate],
        parameters=[{'cmd_vel_topic': '/turtle5/cmd_vel'}]    
    )

    SpiralTurtle3 = Node(
        package='seminar_3',
        executable='task1_spiral',
        name='spiral3',
        arguments=[radius, growth_rate],
        parameters=[{'cmd_vel_topic': '/turtle4/cmd_vel'}]    
    )
    
    SpiralTurtle2 = Node(
        package='seminar_3',
        executable='task1_spiral',
        name='spiral2',
        arguments=[radius, growth_rate],
        parameters=[{'cmd_vel_topic': '/turtle3/cmd_vel'}]    
    )
    
    return LaunchDescription([
        TurtleSim, 
        spawn_turtle2, 
        spawn_turtle3, 
        spawn_turtle4, 
        spawn_turtle5, 
        SpiralTurtle1, 
        SpiralTurtle2, 
        SpiralTurtle3
    ])