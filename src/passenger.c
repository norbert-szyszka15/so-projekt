/* passenger.c */
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>    

void *passengerThread(void *arg) {
   
}


int main() {
    srand(time(NULL));

    // Przyłączenie do pamięci współdzielonej
    shmId = shmget(IPC_PRIVATE, sizeof(SharedData), 0666);
    if (shmId < 0) {
        perror("shmget failed");
        exit(1);
    }

    sharedData = (SharedData *)shmat(shmId, NULL, 0);
    if (sharedData == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    pthread_t passengers[MAX_PAX_ON_TERMINAL];
    int ids[MAX_PAX_ON_TERMINAL];

    for (int i = 0; i < MAX_PAX_ON_TERMINAL; i++) {
        ids[i] = i + 1;
        pthread_create(&passengers[i], NULL, passengerThread, &ids[i]);
        sleep(rand() % 2);
    }

    for (int i = 0; i < MAX_PAX_ON_TERMINAL; i++) {
        pthread_join(passengers[i], NULL);
    }

    // Odłączenie od pamięci współdzielonej
    if (shmdt(sharedData) == -1) {
        perror("shmdt failed");
    }

    return 0;
}
