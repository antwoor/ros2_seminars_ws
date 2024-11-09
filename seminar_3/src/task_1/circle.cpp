#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtle_circle.h"

int main(int argc, char** argv)
{
    const int radius = std::stoi(argv[1]);
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CirclePublisher>(radius));
    rclcpp::shutdown();
}