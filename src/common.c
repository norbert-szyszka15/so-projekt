#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

sem_t controlStations[MAX_SLOTS];
sem_t stairs;
sem_t planeSeats;
volatile bool terminateSimulation = false;

void initialize_sync() {
    for (int i = 0; i < MAX_SLOTS; i++) {
        sem_init(&controlStations[i], 0, 2); // 2 osoby na stanowisko
    }
    sem_init(&stairs, 0, MAX_STAIR_CAPACITY);
    sem_init(&planeSeats, 0, MAX_PASSENGERS);
}

void cleanup_sync() {
    for (int i = 0; i < MAX_SLOTS; i++) {
        sem_destroy(&controlStations[i]);
    }
    sem_destroy(&stairs);
    sem_destroy(&planeSeats);
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Zakończenie symulacji...\n");
        terminateSimulation = true;
    }
}
