#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handle_sigint(int sig) {
    printf("\nDispatcher: Stop signal received. Sending stop signal to all processes.\n");
    sharedData->signalStop = 1;
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);

    initializeSharedMemory();

    while (1) {
        sleep(10); // Co 10 sekund sprawdzanie statusu

        pthread_mutex_lock(&sharedData->boardingGateLock);
        printf("Dispatcher: Checking status of plane %d.\n", sharedData->planes[sharedData->currentPlane].planeID);

        // Jeśli samolot jest pełny, wyślij sygnał takeoff
        if (sharedData->currentPassengers == sharedData->planes[sharedData->currentPlane].maxCapacity) {
            printf("Dispatcher: Plane %d is full. Sending takeoff signal.\n", sharedData->planes[sharedData->currentPlane].planeID);
            sharedData->signalTakeoff = 1;
        }

        pthread_mutex_unlock(&sharedData->boardingGateLock);

        // Jeśli sygnał zatrzymania jest ustawiony, zakończ
        if (sharedData->signalStop) {
            printf("Dispatcher: Stop signal detected. Exiting...\n");
            break;
        }
    }

    cleanupSharedMemory();
    return 0;
}
