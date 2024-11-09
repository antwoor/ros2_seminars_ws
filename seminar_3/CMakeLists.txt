cmake_minimum_required(VERSION 3.5)
project(seminar_3)

#Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
#interesting
add_executable(turtle_cmd_pub src/task_1/turtle_pub.cpp src/task_1/sin.cpp)
ament_target_dependencies(turtle_cmd_pub rclcpp geometry_msgs)
#circle
add_executable(task1_circle src/task_1/turtle_circle.cpp src/task_1/turtle_pub.cpp src/task_1/circle.cpp)
ament_target_dependencies(task1_circle rclcpp geometry_msgs)
#spiral
add_executable(task1_spiral src/task_1/turtle_spiral.cpp src/task_1/turtle_pub.cpp src/task_1/ln_spiral.cpp)
ament_target_dependencies(task1_spiral rclcpp geometry_msgs)

install(TARGETS
  task1_spiral
  task1_circle  
  turtle_cmd_pub
  DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})
  
ament_package()
