#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#define MAX_PAX_ON_TERMINAL 500
#define MAX_AIRCRAFT_CAPACITY 50
#define MAX_BAGGAGE_WEIGHT 10
#define MAX_VIP_PASSENGERS 5
#define MAX_QUEUE_PASS_PER_PAX 3
#define GATES_CAPACITY 10
#define MAX_NUM_OF_PLANES 5

#define SHM_KEY 1234

typedef struct {
    long messageType;
    int passengerID;
    int isVIP;
    int baggageWeight;
    char gender;
} PassengerMessage;

typedef struct {
    int planeID;
    int maxBaggageWeight;
    int maxCapacity;
} Plane;

typedef struct {
    sem_t securityControl;
    pthread_mutex_t securityControlLock;

    sem_t boardingGate;
    pthread_mutex_t boardingGateLock;

    Plane planes[MAX_NUM_OF_PLANES];

    volatile int currentPlane;
    volatile int currentPassengers;    

    volatile int signalTakeoff;
    volatile int signalStop;
} SharedData;

extern int shmId;
extern SharedData *sharedData;

void initializeSharedMemory();
void cleanupSharedMemory();
