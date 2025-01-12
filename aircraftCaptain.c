#include "commons.h"

#include <stdio.h>
#include <unistd.h>

int main() {
   while (1) {
      sleep(30);
      pthread_mutex_lock(&boardingGateLock);
      if (currentPassengers > 0) {
         printf("Captain: Taking off with %d passengers on plane %d.", currentPassengers, planes[currentPlane].planeID);
         currentPassengers = 0;
         currentPlane = (currentPlane + 1) % MAX_NUM_OF_PLANES;
      } else {
         printf("Captain: No passengers. Waiting for the next flight.");
      }
      pthread_mutex_unlock(&boardingGateLock);
   }
   return 0;
}
