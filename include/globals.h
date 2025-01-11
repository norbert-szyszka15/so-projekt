#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_PASSENGERS 186
#define MAX_GATES 5

typedef struct {
    int id;
    int luggageWeight;
    char gender[10];
    bool isVip;
} Passenger;

extern int aircraftPassengerLimit;

extern int boardingStairsCapacity;