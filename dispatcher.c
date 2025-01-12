#include "commons.h"

#include <stdio.h>
#include <unistd.h>

int main() {
   while (1) {
      sleep(10);
      printf("Dispatcher: Announcing boarding readiness for plane %d.", planes[currentPlane].planeID);
   }
   return 0;
}
