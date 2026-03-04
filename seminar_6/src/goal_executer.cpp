#include "goal_seeker.h"

int main(int argc, char** argv) {
    const int x = std::stof(argv[1]);
    const int y = std::stof(argv[2]);
    rclcpp::init(argc, argv);
    auto node = std::make_shared<GoalSeeker>(x,y);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}