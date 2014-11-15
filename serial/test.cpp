#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "serial.h"

using namespace cv;
using namespace std;

int getAcc(struct serial_t *connection) {
  char *start, *end;
  char *buf = connection->readbuf;
  char asint[16];
  start = strrchr(buf, ",") + 1;
  end = strchr(buf, "]");
  memcpy(asint, start, end - start);
  asint[end - start] = '\0';
  return atoi(asint);
}

int main() {
  Mat image;
  Mat s1, s2, s3;
  int index = 0;
  float init_accel[500];

  image.create(Size(720, 480), CV_8UC3);
  s1.create(Size(240, 480), CV_8UC3);
  s2.create(Size(240, 480), CV_8UC3);
  s3.create(Size(240, 480), CV_8UC3);

  struct serial_t connection;
  if (serial_connect(&connection, "/dev/ttyACM1", 115200, 0) == -1)
    return -1;
  printf("connected\n");

  for (int i = 0; i < 500; i++) {
    init_accel[i] = getAcc(&connection);
  }

  
  
  return 0;
}
