#ifndef __decepticon_hpp__
#define __decepticon_hpp__

#include <opencv2/core/core.hpp>

class Decepticon {
  public:
    Decepticon();
    ~Decepticon();

    void set_left();
    void set_right();
    void set_claw();

    void forward();
    void backward();
    void turn_left();
    void turn_right();
    void stop();
    void open_claw();
    void close_claw();

    cv::Mat takePicture();
    int sonar_distance();

  private:
    VideoCapture camera;
    struct serial_t commlink;
    int leftSpeed;
    int rightSpeed;
    int clawPos;
};

#endif
