#include <stdio.h>
#include "decepticon.hpp"

using namespace cv;

/** Constructor
 */
Decepticon::Decepticon() {
  this->left_speed = 0;
  this->right_speed = 0;
  this->claw_pos = 0;
  this->camera = raspiCamCvCreateCameraCapture(CAMERA_DEV);
  serial_connect(&this->commlink, NULL, BAUDRATE);
  if (this->commlink.connected)
    send_to_arduino();
}

/** Deconstructor
 */
Decepticon::~Decepticon() {
  if (this->commlink.connected) {
    stop();
    serial_disconnect(&this->commlink);
  }
  if (this->camera)
    raspiCamCvReleaseCapture(&this->camera);
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
Mat Decepticon::take_picture() {
  Mat picture;
  if (!this->camera)
    this->camera = raspiCamCvCreateCameraCapture(CAMERA_DEV);
  if (!this->camera) /* stop trying and give up */
    return picture;
  picture = raspiCamCvQueryFrame(this->camera);
  return picture;
}

/** Get a message from the arudino.
 *  @return a string of the message
 */
char *Decepticon::get_arduino_message() {
  char *message;
  message = serial_read(&this->commlink);
  if (strlen(message) == 0)
    return NULL;
  return message;
}

/** Take a measurement of the ultrasonic sensor
 *  @return a value representing the distance from the sonar sensor in cm
 */
int Decepticon::get_sonar() {
  int get_sonar_cm;
  get_sonar_cm = 0;
  sscanf(serial_read(&this->commlink), "%03d",
      &get_sonar_cm);
  return get_sonar_cm;
}

/** Check to see if everything is opened correctly
 *  @return true if everything is open, false otherwise
 */
bool Decepticon::opened() {
  bool isopen;
  if (!(isopen = (this->camera != NULL) && this->commlink.connected))
    printf("camera: %d, commlink: %d\n",
        this->camera != NULL, this->commlink.connected);
  return isopen;
}

/** Send values to the arduino
 */
void Decepticon::send_to_arduino() {
  char sig[3];
  sig[1] = '\n';
  sig[2] = '\0';
  sig[0] = 0x40;
  sig[0] = (this->left_speed < 0) ? (sig[0] | 0x10):
    ((this->left_speed > 0) ? (sig[0] | 0x20) : sig[0]);
  sig[0] = (this->right_speed > 0) ? (sig[0] | 0x08) :
    ((this->right_speed < 0) ? (sig[0] | 0x04) : sig[0]);
  sig[0] = (this->claw_pos == 180) ? (sig[0] | 0x02) :
    ((this->claw_pos == 90) ? (sig[0] | 0x01) : sig[0]);

  serial_write(&this->commlink, sig);
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
