#ifndef PASSENGER_H
#define PASSENGER_H

#include "common.h"

#include <semaphore.h>

void passenger_process(int shmID, sem_t* semaphores, int passengerID);
int verify_baggage(int weight, int isVip);

#endif