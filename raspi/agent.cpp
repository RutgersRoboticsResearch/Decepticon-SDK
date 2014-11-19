#include "decepticon/decepticon.hpp"
#include "controller/controller.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

using namespace cv;

int limit_motor(int x) {
  return x < -255 ? -255 : (x > 255 ? 255 : x);
}

int main() {
    Decepticon d;
    struct controller_t controller;
    if (d.opened())
      printf("Connection to the Decepticon established.\n");
    else
      printf("Error: Cannot connect to the Decepticon.\n");

    printf("Controller connected: %s\n", controller.connected ? "true" : "false");

    /*controller_connect(&controller);
    while (controller.connected) {
      float claw_remap;

      d.set_left(limit_motor((int)(
          controller.LJOY.y * 255 +
          controller.RJOY.x * 255)));
      d.set_right(limit_motor((int)(
          controller.LJOY.y * 255 -
          controller.RJOY.x * 255)));

      claw_remap = (controller.RB + 1.0) / 2;
      d.set_claw((int)(claw_remap * 90));
    }
    controller_disconnect(&controller);*/

    namedWindow("camera", CV_WINDOW_AUTOSIZE);
    for (;;) {
      int c;
      /*printf(
          "0: stop\n"
          "1: forward\n"
          "2: backward\n"
          "3: open claw\n"
          "4: close claw\n"
          "5: show camera\n"
          "6: show sonarcm\n"
          "7: quit\n"
          "Please enter command: ");
      scanf("%d", &c);*/
      c = 5;
      if (c == 7)
        break;
      switch (c) {
        case 0:
          d.stop();
          break;
        case 1:
          d.forward();
          break;
        case 2:
          d.backward();
          break;
        case 3:
          d.open_claw();
          break;
        case 4:
          d.close_claw();
          break;
        case 5:
          {
            Mat img = d.takePicture();
            imshow("camera", img);
            if (waitKey(1) & 0x0f == 'q')
              return 0;
            img.release();
          }
          break;
        case 6:
          printf("sonar (cm): %d\n", d.sonar_distance());
          break;
        default:
          d.stop();
          printf("Invalid command.\n");
      }
    }
    destroyAllWindows();
    return 0;
}
