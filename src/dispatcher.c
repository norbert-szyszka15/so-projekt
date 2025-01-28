#include "common.h"
#include "dispatcher.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

void dispatcher_process(int shmID, sem_t* semaphores) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        sleep(10);
        printf("Dyspozytor: sygnał dla wcześniejszego odlotu samolotu.\n");
        sharedData->terminateSimulation = true;

        // Resetowanie symulacji
        sharedData->passengersInQueue = 0;
        sharedData->passengersOnStairs = 0;
        sharedData->passengersInPlane = 0;
        for (int i = 0; i < MAX_SLOTS; i++) {
            sharedData->currentGender[i] = -1;
        }
        TAILQ_INIT(&sharedData->queue);
    }

    shmdt(sharedData);
}