#include "turtle_spiral.h"

SpiralPublisher::SpiralPublisher(const int& a, const int& k) : TrajectoryPublisher() {
    this->a_ = a;
    this->k_ = k;
    this->phi_ = 0;
    this->message.linear.x = 1; // linear velocity
    //this->message.angular.z = this->message.linear.x/radius; // calc angular velocity
}

void SpiralPublisher::timer_callback() {
    r_ = a_ * std::exp(k_ * phi_);
    this->message.angular.z = 1/r_;
    RCLCPP_INFO(this->get_logger(), "driving turtle %f, %f", r_, phi_);
    _pub->publish(this->message);
    phi_ +=0.01;
}
