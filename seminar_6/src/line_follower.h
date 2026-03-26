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
  float angularVel_ = 0.15;    // Базовая угловая скорость поворота (рад/с)
  
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
  
  // Позиция области интереса (ROI) на изображении камеры:
  int row_ = 600;      // Вертикальная координата верхнего края ROI (в пикселях)
  int column_ = 600;   // Горизонтальная координата левого края ROI (в пикселях)
  
  double dt_ = 0.0001;
  double integral = 0;
  
protected:
  virtual void cameraCallback(const sensor_msgs::msg::Image::SharedPtr cameraMsg);
};