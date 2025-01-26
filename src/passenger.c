#include "common.h"
#include "passenger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* passenger_routine(void* arg) {
    Passenger* p = (Passenger*)arg;

    if (p->bagageWeight > MAX_WEIGHT) {
        printf("Pasażer %d odrzucony (waga bagażu).\n", p->id);
        return NULL;
    }

    for (int i = 0; i < MAX_SLOTS; i++) {
        if (sem_trywait(&controlStations[i]) == 0) {
            printf("Pasażer %d kontrolowany na stanowisku %d.\n", p->id, i + 1);
            sleep(1);
            sem_post(&controlStations[i]);
            break;
        }
    }

    sem_wait(&stairs);
    printf("Pasażer %d wchodzi na pokład.\n", p->id);
    sem_post(&stairs);

    sem_wait(&planeSeats);
    printf("Pasażer %d zajął miejsce w samolocie.\n", p->id);

    return NULL;
}

Passenger* create_passenger(int id) {
    Passenger* p = (Passenger*)malloc(sizeof(Passenger));
    p->id = id;
    p->bagageWeight = rand() % (MAX_WEIGHT + 10);
    p->isVip = (id <= MAX_VIP_PASSENERS);
    p->gender = (id % 2 == 0) ? 'M' : 'F';
    return p;
}
