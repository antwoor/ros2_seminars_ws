#include "turtle_pub.h"
TrajectoryPublisher::TrajectoryPublisher() : Node("turtle_comander"){
    this->message.linear.x =0.5;
    this->declare_parameter<std::string>("cmd_vel_topic", "turtle1/cmd_vel");
    std::string velocity = this->get_parameter("cmd_vel_topic").as_string();
    _pub = this->create_publisher<geometry_msgs::msg::Twist>(velocity, 10);
    _timer = this->create_wall_timer(
        500ms,
        std::bind(&TrajectoryPublisher::timer_callback, this)
    );    
}
void TrajectoryPublisher::timer_callback(){
    static double sin_X = 0;
    sin_X > 2 * M_PI ? sin_X = 0: sin_X += 0.1;
    RCLCPP_INFO(this->get_logger(), "driving turtle %f", sin_X);
    this->message.angular.z = sin(sin_X);
    _pub->publish(this->message);
}

// int main(int argc, char** argv)
// {
//     rclcpp::init(argc, argv);
//     rclcpp::spin(std::make_shared<TrajectoryPublisher>());
//     rclcpp::shutdown();
// }