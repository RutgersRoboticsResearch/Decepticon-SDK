#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "decepticon/decepticon.hpp"
#include "controller/controller.h"

using namespace cv;

static int exit_signal;

void quit(int signo) {
  exit_signal = 1;
}

int limit_motor(int speed) {
  int tolerance = 32;
  speed = (speed > tolerance) ? speed : (speed < -tolerance ? speed : 0);
  return (speed > 255) ? 255 : (speed < -255 ? -255 : speed);
}

int sign(int x) {
  return x < 0 ? -1 : 1;
}

int discretize(int signal, int tolerance) {
  return (abs(signal) > tolerance) * sign(signal);
}

int main() {
  Decepticon d;
  struct controller_t controller;
  Mat image;
  int tolerance;

  tolerance = 0.5;
  signal(SIGINT, quit);
  controller_connect(&controller);

  printf("starting to communicate...\n");
  //namedWindow("frame", CV_WINDOW_AUTOSIZE);
  d.stop();
  while (!exit_signal) {
    int leftside;
    int rightside;
    int clawpos;

    leftside = limit_motor(255 * (
        discretize(controller.LJOY.y, tolerance) +
        discretize(controller.RJOY.x, tolerance)));
    rightside = limit_motor(255 * (
        discretize(controller.LJOY.y, tolerance) -
        discretize(controller.RJOY.x, tolerance)));
    clawpos = (int)((-controller.RT + 1.0) / 2.0 * 90.0);

    d.set_left(leftside);
    d.set_right(rightside);
    d.set_claw(clawpos);

    printf("%d\n", d.get_sonar());

    image = d.take_picture();
    //imshow("frame", image);
    //waitKey(1);
  }
  d.stop();

  controller_disconnect(&controller);

  return 0;
}
