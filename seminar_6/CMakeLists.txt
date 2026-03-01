cmake_minimum_required(VERSION 3.8)
project(module_5_assignment)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(CURL REQUIRED)
find_package(TIFF REQUIRED)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(nav_msgs REQUIRED)

add_executable(lineFollowing src/line_follower.cpp src/line_following_with_camera.cpp)
ament_target_dependencies(lineFollowing rclcpp sensor_msgs cv_bridge geometry_msgs)

add_executable(GoalSeeker src/goal_seeker.cpp src/goal_executer.cpp)
ament_target_dependencies(GoalSeeker rclcpp sensor_msgs nav_msgs geometry_msgs tf2_ros)

target_link_libraries(lineFollowing ${OpenCV_LIBS})
target_link_libraries(lineFollowing ${CURL_LIBRARIES})
target_link_libraries(lineFollowing TIFF::TIFF)

install(TARGETS lineFollowing GoalSeeker
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch worlds urdf models
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
