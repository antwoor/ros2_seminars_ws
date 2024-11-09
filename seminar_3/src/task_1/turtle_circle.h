#include "turtle_pub.h"

class CirclePublisher : public TrajectoryPublisher {
public:
    CirclePublisher(const int& radius);
protected:
    void timer_callback() override; 
};
