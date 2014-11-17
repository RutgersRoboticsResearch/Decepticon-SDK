#ifndef __decepticon_hpp__
#define __decepticon_hpp__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "serial.h"

#define BAUDRATE 115200

class Decepticon {
  public:
    Decepticon();
    ~Decepticon();

    void set_left(int speed);
    void set_right(int speed);
    void set_claw(int position);

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
    cv::VideoCapture camera;
    struct serial_t commlink;
    int left_speed;
    int right_speed;
    int claw_pos;

    void send_to_arduino();
    int bound(int signal, int min, int max);
};

#endif
