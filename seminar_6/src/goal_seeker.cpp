#include "goal_seeker.h"


GoalSeeker::GoalSeeker(float X, float Y): Node("GoalSeekerNode"), X_goal(X), Y_goal(Y) {
    subscription_ = this->create_subscription<nav_msgs::msg::Odometry>("/odom", 10, std::bind(&GoalSeeker::odom_callback, this, std::placeholders::_1));
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    this->declare_parameter<double>("kP", 0.01);
    this->declare_parameter<double>("kI", 0.0006);
    p = this->get_parameter("kP").as_double();
    i = this->get_parameter("kI").as_double();
}
void GoalSeeker::odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    RCLCPP_INFO(this->get_logger(), "Received odometry message");
    auto x = msg->pose.pose.orientation.x;
    auto y = msg->pose.pose.orientation.y;
    auto z = msg->pose.pose.orientation.z;
    auto w = msg->pose.pose.orientation.w;

    static double yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));
    static double self_x = msg->pose.pose.position.x;
    static double self_y = msg->pose.pose.position.y;
    static double angle_error = atan2(Y_goal-self_y, X_goal-self_x) -yaw;
    static double linear_error = sqrt(pow(X_goal, 2)+pow(Y_goal,2));
    if (abs(angle_error) <= 0.01)
    {
        if(linear_error>=0.01){move2goal(linear_error);}
    } 
    else
    {
        rotate2goal(angle_error);
    }
    publisher_->publish(this->velocityMsg);
}

void GoalSeeker::rotate2goal(const double& error)
{
    this->velocityMsg.angular.z = angularVel_ * (p*error + i*integral);
}

void GoalSeeker::move2goal(const double& error)
{   
    this->velocityMsg.linear.x = linearVel_ * (p*error + i*integral);
}
