#include "turtle_circle.h"

CirclePublisher::CirclePublisher(const int& radius) : TrajectoryPublisher() {
    this->message.linear.x = 1; // linear velocity
    this->message.angular.z = this->message.linear.x/radius; // calc angular velocity
}

void CirclePublisher::timer_callback() {
    
    _pub->publish(this->message);
}
