#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath>

using namespace std::chrono_literals;
class TrajectoryPublisher : public rclcpp::Node
{
    public:
    TrajectoryPublisher();
    virtual ~TrajectoryPublisher() = default; // Виртуальный деструктор
    private:
    rclcpp::TimerBase::SharedPtr _timer =NULL;
    protected:
    virtual void timer_callback();
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr _pub = NULL;
    geometry_msgs::msg::Twist message;
};