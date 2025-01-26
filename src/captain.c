#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handle_sigint(int sig) {
    printf("\nCaptain: Stop signal received. Ending operations.\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    // Przyłączenie do pamięci współdzielonej
    shmId = shmget(SHM_KEY, sizeof(SharedData), 0666);
    if (shmId < 0) {
        perror("shmget failed");
        exit(1);
    }

    sharedData = (SharedData *)shmat(shmId, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    while (1) {
        pthread_mutex_lock(&sharedData->boardingGateLock);

        // Sprawdź sygnał zatrzymania
        if (sharedData->signalStop) {
            printf("Captain: Stop signal received. Ending operations.\n");
            pthread_mutex_unlock(&sharedData->boardingGateLock);
            break;
        }

        // Odlot po sygnale takeoff lub gdy pasażerowie są na pokładzie
        if (sharedData->signalTakeoff || sharedData->currentPassengers > 0) {
            printf("Captain: Taking off with %d passengers on plane %d.\n", sharedData->currentPassengers, sharedData->planes[sharedData->currentPlane].planeID);
            sharedData->currentPassengers = 0; // Reset liczby pasażerów
            sharedData->currentPlane = (sharedData->currentPlane + 1) % MAX_NUM_OF_PLANES; // Przełącz na następny samolot
            sharedData->signalTakeoff = 0; // Reset sygnału takeoff
        } else {
            printf("Captain: No passengers. Waiting for the next flight.\n");
        }

        pthread_mutex_unlock(&sharedData->boardingGateLock);
        sleep(5); // Krótszy czas reakcji
    }

    if (shmdt(sharedData) == -1) {
        perror("shmdt failed");
    }

    return 0;
}
