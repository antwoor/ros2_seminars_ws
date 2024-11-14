# Module 2 Assignment: Developing Custom ROS 2 Nodes and Launch Files

## Objective

This assignment focuses on developing your ability to write custom ROS 2 nodes and utilize launch files for running multiple nodes simultaneously. You will create a custom ROS 2 node that controls the Turtlesim simulation and develop a launch file to run the simulation and node together.

## Tasks
### Build a docker image
```bash
   cd ros2_seminars_ws
```
```bash
   docker build -t stankin_humble .
```
Now you have a docker image with name of stankin_humble. You can launch it with **`run.sh`** script that locates in **`seminar_3`** folder
```bash
   cd ~/ros2_seminars_ws/seminar_3
   bash run.sh
```

it will open our recently created image with special parametres such as

**`--env DISPLAY=$DISPLAY`** to show graphics to display 

**`--env QT_X11_NO_MITSHM=1`** to display QT-based apps properelly 

**`--volume /tmp/.X11-unix:/tmp/.X11-unix `** to connect our docker graphic apps with **X11** window-display-manager 

**`--volume $PWD:/workspace`** to mount our current directory to docker`s **/workspace** directory  

By the way line xhost **`+local:docker`** command used to give access to docker to use **X11** system
```bash 
   cd /workspace
   colcon build
   source install/setup.bash
   ```

# Task 1: Create or modify a Custom ROS 2 Node

- **Describe the code in src/task1 folder** inspect the code. If you want, write comments :
  - **Circle Movement:** change the radius and velocity.
  - **Logarithmic Spiral Movement:** change the radius and growth rate.

## You can launch a turtlesim_node with this command
```bash
 ros2 run turtlesim turtlesim_node  
 ```
#### and then execute these commands in another terminal to run your nodes
измените цифру 1 на ваш желаемый радиус
```bash
 ros2 run seminar_3 task1_circle 1  
 ```
измените цифры 1 и 1 на ваши желаемые параметры
```bash
 ros2 run seminar_3 task1_spiral 1 1
 ```
## Or you can simply modify ***'launch/task_1.launch.py'***
## To create your custom node you should write **`your_node.cpp`** file in ***'src/task1'*** directory and then add new string to **`CMakeLists.txt`**
```cmake
   add_executable("Your custom node name" src/task_1/"your file.cpp" src/task_1/turtle_pub.cpp src/task_1/"your file with main function".cpp)
ament_target_dependencies(task1_spiral rclcpp geometry_msgs)
``` 
# Task 2: Develop/modify a Launch File

- **Create a launch file** that starts the Turtlesim simulation and the custom ROS 2 node simultaneously with a lots of turtles.

- **Ensure proper documentation** write comments.
```python
   SpiralTurtle1 = Node( #SpiralTurtle1 - name of the node in Python
        package='seminar_3', #name of the package
        executable='task1_spiral', #your executable from CmakeLists.txt
        name='spiral1', #name of the node in rqt_graph
        arguments=[radius, growth_rate],
        parameters=[{'cmd_vel_topic': '/turtle5/cmd_vel'}]    
    )
```
run #example
```bash
ros2 launch seminar_3 task_2.launch.py
```
# Task 3: Use your node to drive turtlebot3
first, run
```bash
export TURTLEBOT3_MODEL=burger
```
then run
```bash
source /opr/ros/humble/setup.bash
ros2 launch turtlebot3_gazebo turtlebot3_world.launch.py
```
and in another terminal run
```bash
ros2 launch seminar_3 turtle3.launch.py      
```
## replace turtle3.launch.py with yor launch file
## Learning Outcome

By completing this assignment, you will:
- Understand how to develop and execute custom nodes in ROS 2.
- Learn the utility of launch files in managing the execution of multiple nodes in ROS 2.
- Learn how to interact with ROS 2 services to modify node behavior and simulation environments.
- Understand how to use ROS 2 parameters to control and alter the behavior of nodes in real-time.
---
## Submission Process
25-34 : отчёт, скриншоты изменённых/новых частей кода, первое задание

35-44 : то же самое что и на 25 + второе задание

45-54 : запустить свой код на имуляции реального робота(задание номер 3)  