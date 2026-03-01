// Purpose:
// - Receives Image messages, applies Canny edge detection, calculates the error between midpoint and center,
//   and publishes Twist messages to control robot motion.
// - Demonstrates image processing and motion control integration in a ROS2 environment.
// Author: Robotisim, Antwoor

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"

class LineFollower : public rclcpp::Node
{
public:
  LineFollower();
  ~LineFollower(){};

private:
  float angularVel_ = 0.3;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  int row_ = 600;
  int column_ = 600;
protected:
  virtual void cameraCallback(const sensor_msgs::msg::Image::SharedPtr cameraMsg);
};
