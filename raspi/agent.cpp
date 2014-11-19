#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "decepticon/decepticon.hpp"

using namespace cv;


int main() {
    Decepticon d;
    if (d.opened()) {
      printf("Connection to the Decepticon established.\n");
    } else {
      printf("Error: Cannot connect to the Decepticon.\n");
      return -1;
    }

    /* AGENT PROGRAM HERE */

    return 0;
}
