#include "common.h"
#include "dispatcher.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>

void dispatcher_process(int shmID, sem_t* semaphores) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        sleep(10);
        printf("Dyspozytor: sprawdzanie stanu samolotów.\n");

        // Check if there are any passengers waiting or boarding
        bool passengersWaiting = false;
        for (int i = 0; i < NUM_GATES; i++) {
            if (sharedData->passengersInQueue > 0 || sharedData->passengersInPlanes[i] > 0) {
                passengersWaiting = true;
                break;
            }
        }

        if (!passengersWaiting) {
            printf("Dyspozytor: brak pasażerów, kończenie symulacji.\n");
            sharedData->terminateSimulation = true;

            // Send termination signal to all child processes
            kill(0, SIGTERM);
        }
    }

    shmdt(sharedData);
    exit(0); // Ensure the process exits cleanly
}