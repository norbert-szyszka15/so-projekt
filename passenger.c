#include "commons.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

void *passengerThread(void *arg) {
   int passengerID = *(int *)arg;
   int isVIP = rand() % 2;
   int baggageWeight = rand() % (planes[currentPlane].maxBaggageWeight + 5);
   char gender = (rand() % 2 == 0) ? 'M' : 'F';
   int queuePassed = 0;

   if (baggageWeight > planes[currentPlane].maxBaggageWeight) {
      printf("Pasażer %d: waga bagażu przekracza dopuszczalny limit. Powrót do terminala.\n", passengerID);
      return NULL;
   }

   while (true) {
      pthread_mutex_lock(&securityControlLock);
      if (queuePassed < MAX_QUEUE_PASS_PER_PAX) {
         queuePassed++;
         pthread_mutex_unlock(&securityControlLock);
         sleep(1);
      } else {
         pthread_mutex_unlock(&securityControlLock);
         printf("Pasażer %d: pasażer sfrustrował się zbyt długim czasem oczekiwania. Powrót do terminala.\n", passengerID);
         return NULL;
      }

      sem_wait(&securityControl);
      sleep(1);
      sem_post(&securityControl);
      break;
   }

   sem_wait(&boardingGate);
   pthread_mutex_lock(&boardingGateLock);
   if (currentPassengers < planes[currentPlane].maxCapacity) {
      currentPassengers++;
   } else {
      printf("Pasażer %d: na pokładzie samolotu nie ma więcej miejsc. Powrót do terminala.\n", passengerID);
   }
   pthread_mutex_unlock(&boardingGateLock);
   sem_post(&securityControl);

   return NULL;
}

int main() {
   srand(time(NULL));

   pthread_t passengers[MAX_PAX_ON_TERMINAL];
   int ids[MAX_PAX_ON_TERMINAL];
   
   sem_init(&securityControl, 0, 6);
    sem_init(&boardingGate, 0, GATES_CAPACITY);
    pthread_mutex_init(&boardingGateLock, NULL);
    pthread_mutex_init(&securityControlLock, NULL);

    for (int i = 0; i < MAX_NUM_OF_PLANES; i++) {
        planes[i].planeID = i + 1;
        planes[i].maxBaggageWeight = MAX_BAGGAGE_WEIGHT + i * 2;
        planes[i].maxCapacity = MAX_AIRCRAFT_CAPACITY;
    }

    for (int i = 0; i < MAX_PAX_ON_TERMINAL; i++) {
        ids[i] = i + 1;
        pthread_create(&passengers[i], NULL, passengerThread, &ids[i]);
        sleep(rand() % 2);
    }

    for (int i = 0; i < MAX_PAX_ON_TERMINAL; i++) {
        pthread_join(passengers[i], NULL);
    }

    sem_destroy(&securityControl);
    sem_destroy(&boardingGate);
    pthread_mutex_destroy(&boardingGateLock);
    pthread_mutex_destroy(&securityControlLock);

   return 0;
}
