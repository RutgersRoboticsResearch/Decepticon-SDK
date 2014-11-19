#include <stdio.h>
#include "decepticon.hpp"

using namespace cv;

/** Constructor
 */
Decepticon::Decepticon() {
    this->left_speed = 0;
    this->right_speed = 0;
    this->claw_pos = 0;
    this->camera.open(0);
    serial_connect(&this->commlink, NULL, BAUDRATE, 0);
}

/** Deconstructor
 */
Decepticon::~Decepticon() {
  serial_disconnect(&this->commlink);
  if (camera.isOpened())
    camera.release();
}

/** Set the left-hand side speed
 *  @param speed
 *    a value from -255 (full backward) to 255 (full forward)
 */
void Decepticon::set_left(int speed) {
  this->left_speed = limit_signal(speed, -255, 255);
  send_to_arduino();
}

/** Set the right-hand side speed
 *  @param speed
 *    a value from -255 (full backward) to 255 (full forward)
 */
void Decepticon::set_right(int speed) {
  this->right_speed = limit_signal(speed, -255, 255);
  send_to_arduino();
}

/** Set the claw position
 *  @param position
 *    a value from 0 (fully closed) to 180 (fully open)
 */
void Decepticon::set_claw(int position) {
  this->claw_pos = limit_signal(position, 0, 180);
  send_to_arduino();
}

/** Make robot go full speed forward
 */
void Decepticon::forward() {
  this->left_speed = 255;
  this->right_speed = 255;
  send_to_arduino();
}

/** Make the robot go full speed backward
 */
void Decepticon::backward() {
  this->left_speed = -255;
  this->right_speed = -255;
  send_to_arduino();
}

/** Make the robot turn left at full speed
 */
void Decepticon::turn_left() {
  this->left_speed = -255;
  this->right_speed = 255;
  send_to_arduino();
}

/** Make the robot turn right at full speed
 */
void Decepticon::turn_right() {
  this->left_speed = 255;
  this->right_speed = -255;
  send_to_arduino();
}

/** Make the robot stop in place
 */
void Decepticon::stop() {
  this->left_speed = 0;
  this->right_speed = 0;
  send_to_arduino();
}

/** Make the robot open the claw
 */
void Decepticon::open_claw() {
  this->claw_pos = 90;
  send_to_arduino();
}

/** Make the robot close the claw
 */
void Decepticon::close_claw() {
  this->claw_pos = 0;
  send_to_arduino();
}

/** Take a picture using the camera
 *  @return a matrix representing the picture
 */
Mat Decepticon::takePicture() {
    Mat picture;
    if (!camera.isOpened())
      camera.open(0);
    if (!camera.isOpened()) /* stop trying and give up */
      return picture;
    camera >> picture;
    return picture;
}

/** Take a measurement of the ultrasonic sensor
 *  @return a value representing the distance from the sonar sensor in cm
 */
int Decepticon::sonar_distance() {
    int sonar_distance_cm;
    sonar_distance_cm = 0;
    sscanf(serial_read(&this->commlink), "%03d",
        &sonar_distance_cm);
    return sonar_distance_cm;
}

/** Check to see if everything is opened correctly
 *  @return true if everything is open, false otherwise
 */
bool Decepticon::opened() {
  return camera.isOpened() && commlink.connected;
}

/** Send values to the arduino
 */
void Decepticon::send_to_arduino() {
  int l, r, c;
  l = this->left_speed < 0 ? -this->left_speed + 255 : this->left_speed;
  r = this->right_speed < 0 ? -this->right_speed + 255 : this->right_speed;
  c = this->claw_pos;
  sprintf(this->buf, "%03d%03d%03d\n", l, r, c);
  serial_write(&this->commlink, this->buf);
}

/** Limit a signal between a min and max
 *  @param signal
 *    the signal to limit
 *  @param min
 *    the lower bound
 *  @param max
 *    the upper bound
 *  @return the limited signal
 */
int Decepticon::limit_signal(int signal, int min, int max) {
  return (signal > max) ? max : (signal < min ? min : signal);
}
