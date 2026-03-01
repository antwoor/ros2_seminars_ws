// Purpose:
// - Receives Image messages, applies Canny edge detection, calculates the error between midpoint and center,
//   and publishes Twist messages to control robot motion.
// - Demonstrates image processing and motion control integration in a ROS2 environment.
// Author: Robotisim, Antwoor

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

class GoalSeeker : public rclcpp::Node
{
public:
  GoalSeeker(float X, float Y);
  ~GoalSeeker(){};

private:
  float X_goal, Y_goal;
  float angularVel_ = 0.1;
  float linearVel_ = 0.1;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
  double dt_=0.0001;
  double integral = 0;
  double error = 0.0;
  double p,i;
public:
  void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg);
  void rotate2goal(const double& error);
  void move2goal(const double& error);
  void calc_error();
  geometry_msgs::msg::Twist velocityMsg;
};
