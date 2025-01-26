#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>

int shmId;
SharedData *sharedData;

void initializeSharedMemory() {
    shmId = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmId < 0) {
        perror("shmget failed");
        exit(1);
    }
    printf("Dispatcher: shmId: %d\n", shmId);

    sharedData = (SharedData *)shmat(shmId, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    for (int i = 0; i < MAX_NUM_OF_PLANES; i++) {
    sharedData->planes[i].planeID = i + 1;
    sharedData->planes[i].maxBaggageWeight = MAX_BAGGAGE_WEIGHT + i * 2; // Dopuszczalne limity
    sharedData->planes[i].maxCapacity = MAX_AIRCRAFT_CAPACITY;           // Pojemność samolotu
    }

    memset(sharedData, 0, sizeof(SharedData));

    sem_init(&sharedData->securityControl, 1, 6);
    pthread_mutex_init(&sharedData->securityControlLock, NULL);

    sem_init(&sharedData->boardingGate, 1, GATES_CAPACITY);
    pthread_mutex_init(&sharedData->boardingGateLock, NULL);

    sharedData->currentPlane = 0;
    sharedData->currentPassengers = 0;
    sharedData->signalTakeoff = 0;
    sharedData->signalStop = 0;
}

void cleanupSharedMemory() {
    // Niszczenie zasobów współdzielonych
    sem_destroy(&sharedData->securityControl);
    pthread_mutex_destroy(&sharedData->securityControlLock);

    sem_destroy(&sharedData->boardingGate);
    pthread_mutex_destroy(&sharedData->boardingGateLock);

    if (shmdt(sharedData) == -1) {
        perror("shmdt failed");
    }

    if (shmctl(shmId, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
    }
}
