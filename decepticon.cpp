#include "decepticon.hpp"

using namespace cv;

Decepticon::Decepticon(){
    left_speed = 0;
    right_speed = 0;
    claw_pos = 0;
    camera.open(0);
    commlink.fd = -1;
    serial_connect(&commlink, NULL, BAUDRATE, 0);
}
Decepticon::~Decepticon(){
  serial_disconnect(&commlink);
}
void Decepticon::set_left(int speed){left_speed = speed;}
void Decepticon::set_right(int speed){right_speed = speed;}
void Decepticon::set_claw(int position){claw_pos = position;}
void Decepticon::forward(){}
void Decepticon::backward(){}
void Decepticon::turn_left(){}
void Decepticon::turn_right(){}
void Decepticon::stop(){}
void Decepticon::open_claw(){}
void Decepticon::close_claw(){}

cv::Mat takePicture(){
    cv::Mat temp;
    return temp;
}
int Decepticon::sonar_distance(){
    return 0;
}
