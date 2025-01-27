#ifndef PASSENGER_H
#define PASSENGER_H

#include "common.h"

#include <semaphore.h>

void passenger_process(int shmID, sem_t* semaphores, int passengerID);
int verify_baggage(int weight, int isVip);
void print_queue(struct passenger_queue *queue);

#endif