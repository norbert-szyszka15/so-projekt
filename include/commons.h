#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_PAX_ON_TERMINAL 100
#define MAX_AIRCRAFT_CAPACITY 50
#define MAX_BAGGAGE_WEIGHT 10
#define MAX_VIP_PASSENGERS 5
#define MAX_QUEUE_PASS_PER_PAX 3
#define GATES_CAPACITY 10
#define MAX_NUM_OF_PLANES 5

typedef struct {
    long messageType;
    int passengerID;
    int isVIP;
    int baggageWeight;
    char gender;
} passengerMessage;

typedef struct {
    int planeID;
    int maxBaggageWeight;
    int maxCapacity;
} Plane;

extern sem_t securityControl;
extern pthread_mutex_t securityControlLock;

extern sem_t boardingGate;
extern pthread_mutex_t boardingGateLock;

extern Plane planes[MAX_NUM_OF_PLANES];

extern int currentPlane;
extern int currentPassengers;