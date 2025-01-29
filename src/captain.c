#include "common.h"
#include "captain.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>

void captain_process(int shmID, sem_t* semaphores, int gateID) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        // Oczekiwanie na załadunek pasażerów
        sleep(30 * (gateID + 1));
        printf("Kapitan %d: samolot gotowy do startu.\n", gateID + 1);

        // Sprawdzenie, czy wszyscy pasażerowie są na pokładzie
        if (sharedData->passengersInPlanes[gateID] > 0) {
            printf("Kapitan %d: samolot startuje z %d pasażerami.\n", gateID + 1, sharedData->passengersInPlanes[gateID]);

            // Lot i powrót
            sleep(3);
            printf("Kapitan %d: samolot wylądował.\n", gateID + 1);

            // Resetowanie liczby pasażerów na pokładzie
            sharedData->passengersInPlanes[gateID] = 0;
        } else {
            printf("Kapitan %d: brak pasażerów na pokładzie, odlot odroczony.\n", gateID + 1);
        }
    }

    shmdt(sharedData);
    exit(0); // Ensure the process exits cleanly
}