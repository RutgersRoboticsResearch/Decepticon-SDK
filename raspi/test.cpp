#include "decepticon.hpp"
#include <stdio.h>

int main() {
    Decepticon d;
    if (d.opened())
      printf("everything opened correctly\n");
    else
      printf("error occured with something\n");
    return 0;
}
