#include "line_follower.h"

LineFollower::LineFollower() : Node("LineFollowingNode") {
    this->declare_parameter<std::string>("camera_topic", "/camera/image_raw");
    std::string camera_topic = this->get_parameter("camera_topic").as_string();
    this->declare_parameter<int>("kP", 1);
    this->declare_parameter<int>("kI", 6);
    this->declare_parameter<int>("lower_threshold", 50);
    this->declare_parameter<int>("upper_threshold", 100);
    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
      camera_topic, 10,
      std::bind(&LineFollower::cameraCallback, this, std::placeholders::_1));
    RCLCPP_INFO(this->get_logger(), "\n------ Node Started -----\n");
};
void LineFollower::cameraCallback(const sensor_msgs::msg::Image::SharedPtr cameraMsg){
    
    auto velocityMsg = geometry_msgs::msg::Twist();
    static cv_bridge::CvImagePtr cvPtr; //Create OpenCv Image pointer
    cvPtr = cv_bridge::toCvCopy(cameraMsg, "bgr8"); //translate ROS cameraMSG to OpenCv image
    cv::Mat grayImage, cannyImage; //creates two colorFrames for the next filtering & scanning operations
    cv::cvtColor(cvPtr->image, grayImage, cv::COLOR_BGR2GRAY); //Change the colorscheme of Image from cvBridge & save it to GrayImage

    //Define the tresholds for Canny
    static int lowerThreshold = this->get_parameter("lower_threshold").as_int();
    static int upperThreshold = this->get_parameter("upper_threshold").as_int();
    double p = (float)this->get_parameter("kP").as_int();
    double i = (float)this->get_parameter("kI").as_int();
    p *=0.01;
    i *=0.0001;
    //Apply Canny filter to find all of the contours 
    cv::Canny(grayImage , cannyImage, lowerThreshold, upperThreshold);
    cv::Mat roi = cannyImage(cv::Range(row_, row_+200), cv::Range(column_, column_+500));
    //
    //Here image processing starts
    //
    static std::vector<int> edges;//here we are checking how many white pixels
    for(int i=0; i<500; ++i){// along the almost whole X-axis of an  croppedimage 
      if(roi.at<uchar>(160,i)==255){// img is already cropped so we have to measure relative numbers of pixels
        edges.push_back(i);
        //RCLCPP_INFO(this->get_logger(), "\n count of  edges: %i \n", i);
      }
    }
    static int midArea;
    static int midPoint;
    static int robotMidPoint;
    if (!edges.empty()) {
      midArea = edges.back() - edges.front();
      midPoint = edges.front() + midArea / 2;
      robotMidPoint = 500 / 2;

      // Calculate error and adjust robot's direction
      double error = robotMidPoint - midPoint;
      integral += error*dt_;
      velocityMsg.linear.x = 0.1;
      velocityMsg.angular.z = angularVel_ * (p*error + i*integral);
      publisher_->publish(velocityMsg);
      RCLCPP_INFO(this->get_logger(), "ERROR = %f , VELOCITY = %f, P_GAIN = %f, I_GAIN = %f, integral = %f", error, velocityMsg.angular.z, p, i, integral);
    }
      // Visualization
      cv::circle(roi, cv::Point(midPoint, 160), 2, cv::Scalar(255, 255, 255), -1);
      cv::circle(roi, cv::Point(robotMidPoint, 160), 5, cv::Scalar(255, 255, 255), -1);
    cv::imshow("Image", roi);
    cv::waitKey(1);
}