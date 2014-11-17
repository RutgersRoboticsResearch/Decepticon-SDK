#ifndef __decepticon_hpp__
#define __decepticon_hpp__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "serial.hpp"

#define BAUDRATE 115200

class Decepticon {
  public:
    Decepticon();
    ~Decepticon();

    /* Easy to use functions */
    void forward();
    void backward();
    void turn_left();
    void turn_right();
    void stop();
    void open_claw();
    void close_claw();

    /* More precise control */
    void set_left(int speed);
    void set_right(int speed);
    void set_claw(int position);

    /* Get camera/ultrasonic data */
    cv::Mat takePicture();
    int sonar_distance();
    bool opened();

  private:
    cv::VideoCapture camera;
    struct serial_t commlink;
    int left_speed;
    int right_speed;
    int claw_pos;
    int sonar_distance_cm;

    void send_to_arduino();
    char buf[256];
    int limit_signal(int signal, int min, int max);
};

#endif
