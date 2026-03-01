#include <geometry_msgs/msg/twist.hpp>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

enum class RobotState {
  MOVING_STRAIGHT,
  TURNING_LEFT,
  TURNING_RIGHT,
  OUT_OF_MAZE
};

class MazeSolving : public rclcpp::Node {
public:
  MazeSolving() : Node("maze_solver"), state_(RobotState::MOVING_STRAIGHT) {
    publisher_ =
        this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10,
        std::bind(&MazeSolving::lidarCallback, this, std::placeholders::_1));
  }

private:
  void lidarCallback(const sensor_msgs::msg::LaserScan::SharedPtr lidarMsg) {
    float rightObstacle = *std::min_element(lidarMsg->ranges.begin() + 260, lidarMsg->ranges.begin() + 280);
    float frontObstacle = *std::min_element(lidarMsg->ranges.begin() + 340, lidarMsg->ranges.begin() + 360);
    float leftObstacle = *std::min_element(lidarMsg->ranges.begin() + 80, lidarMsg->ranges.begin() + 100);

    RCLCPP_INFO(this->get_logger(), "Front: %f, Right: %f, Left: %f",
                frontObstacle, rightObstacle, leftObstacle);

    if (frontObstacle > frontThreshold_ && rightObstacle > 1.2 && leftObstacle > 1.2) {
      state_ = RobotState::OUT_OF_MAZE;
    } 
    geometry_msgs::msg::Twist command;
    if ((frontObstacle > leftObstacle || frontObstacle > rightObstacle) && frontObstacle >frontThreshold_)
    {
      state_ = RobotState::MOVING_STRAIGHT;
    }
    else if (rightObstacle > frontObstacle && rightObstacle > rightThreshold_)
    {
      state_ = RobotState::TURNING_RIGHT;
    }
    else{
      state_ = RobotState::TURNING_LEFT;
    }
    switch (state_) {
    case RobotState::MOVING_STRAIGHT:
      command.linear.x = linearVel_;
      command.angular.z = 0.0;
      RCLCPP_INFO(this->get_logger(), "Moving STRAIGHT");
      break;
    case RobotState::TURNING_LEFT:
      command.linear.x = 0.0;
      command.angular.z = angularVel_;
      RCLCPP_INFO(this->get_logger(), "Moving LEFT");
      break;
    case RobotState::TURNING_RIGHT:
      command.linear.x = 0.0;
      command.angular.z = -angularVel_;
      RCLCPP_INFO(this->get_logger(), "Moving RIGHT");
      break;
    case RobotState::OUT_OF_MAZE:
      command.linear.x =  0.0;
      command.angular.z = 0.0;
      RCLCPP_INFO(this->get_logger(), "OUT OF MAZE");
      break;
    }

    publisher_->publish(command);
  }

  float frontThreshold_ = 1.5f;
  float rightThreshold_ = 0.7f;
  float lefttThreshold_ = 0.7f;
  float angularVel_ = 0.8f;
  float linearVel_ = 0.6f;
  RobotState state_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MazeSolving>());
  rclcpp::shutdown();
  return 0;
}

