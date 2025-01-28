#include "common.h"
#include "captain.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

void captain_process(int shmID, sem_t* semaphores) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        // Oczekiwanie na załadunek pasażerów
        sleep(5);
        printf("Kapitan: samolot gotowy do startu.\n");

        // Sprawdzenie, czy wszyscy pasażerowie są na pokładzie
        if (sharedData->passengersInPlane > 0) {
            printf("Kapitan: samolot startuje z %d pasażerami.\n", sharedData->passengersInPlane);

            // Lot i powrót
            sleep(3);
            printf("Kapitan: samolot wylądował.\n");

            // Resetowanie liczby pasażerów na pokładzie
            sharedData->passengersInPlane = 0;

            // Resetowanie symulacji
            sharedData->passengersInQueue = 0;
            sharedData->passengersOnStairs = 0;
            for (int i = 0; i < MAX_SLOTS; i++) {
                sharedData->currentGender[i] = -1;
            }
            TAILQ_INIT(&sharedData->queue);
        } else {
            printf("Kapitan: brak pasażerów na pokładzie, odlot odroczony.\n");
        }
    }

    shmdt(sharedData);
}