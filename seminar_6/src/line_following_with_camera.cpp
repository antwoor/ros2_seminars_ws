// Purpose:
// - Receives Image messages, applies Canny edge detection, calculates the error between midpoint and center,
//   and publishes Twist messages to control robot motion.
// - Demonstrates image processing and motion control integration in a ROS2 environment.
// Author: Robotisim
#include "line_follower.h"

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LineFollower>());
  rclcpp::shutdown();
  return 0;
}
