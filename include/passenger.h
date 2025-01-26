#ifndef PASSENGER_H
#define PASSENGER_H

typedef struct {
    int id;
    int bagageWeight;
    int isVip;
    char gender;
} Passenger;

void* passenger_routine(void* arg);
Passenger createPassenger(int id);

#endif