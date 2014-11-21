#ifndef __decepticon_hpp__
#define __decepticon_hpp__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "serial.h"
#include "RaspiCamCV.h"

#define CAMERA_DEV  0
#define BAUDRATE    115200

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
    void set_left(int speed);     /* min: -255, max: 255 */
    void set_right(int speed);    /* min: -255, max: 255 */
    void set_claw(int position);  /* min: 0, max: 180 */

    /* Get camera/ultrasonic data */
    cv::Mat take_picture();
    char *get_arduino_message();
    int get_sonar();
    bool opened();

  private:
    RaspiCamCvCapture *camera;
    struct serial_t commlink;
    int left_speed;
    int right_speed;
    int claw_pos;

    void send_to_arduino();
    char buf[256];
    int limit_signal(int signal, int min, int max);
};

#endif
