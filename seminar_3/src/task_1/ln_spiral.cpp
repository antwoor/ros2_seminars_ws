#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtle_spiral.h"

int main(int argc, char** argv)
{
    const int a = std::stod(argv[1]);
    const int k = std::stod(argv[2]);
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SpiralPublisher>(a,k));
    rclcpp::shutdown();
}