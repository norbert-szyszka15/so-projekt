#include "common.h"
#include "passenger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

void passenger_process(int shmID, sem_t* semaphores, int passengerID) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Pasażer %d: rozpoczyna proces odprawy.\n", passengerID);

    // Generowanie wagi bagażu
    int weight = generate_random_weight();
    printf("Pasażer %d: waga bagażu wynosi %d kg.\n", passengerID, weight);

    // Generowanie płci
    int gender = generate_random_gender(); // 0 - mężczyzna, 1 - kobieta
    printf("Pasażer %d: płeć - %s.\n", passengerID, gender == 0 ? "mężczyzna" : "kobieta");

    // Symulacja odprawy
    int isVip = (passengerID % 2 == 0); // VIP co drugi pasażer
    if (!verify_baggage(weight, isVip)) {
        printf("Pasażer %d: odprawa nieudana. Opuścił lotnisko.\n", passengerID);
        return; // Koniec procesu odrzuconego pasażera
    }

    // Wejście do kolejki między odprawą bagażową a kontrolą bezpieczeństwa
    // Zaimplementowane przy pomocy kolejki FIFO
    sem_wait(&semaphores[MAX_SLOTS]); // Dostęp do kolejki
    struct passenger_entry *new_entry = (struct passenger_entry *)malloc(sizeof(struct passenger_entry));
    if (new_entry == NULL) {
        perror("malloc");
        exit(1);
    }
    new_entry->passengerID = passengerID;
    if (isVip) {
        TAILQ_INSERT_HEAD(&sharedData->queue, new_entry, entries); // VIPs go to the front
    } else {
        TAILQ_INSERT_TAIL(&sharedData->queue, new_entry, entries);
    }
    printf("Pasażer %d: dodany do kolejki.\n", passengerID);
    sem_post(&semaphores[MAX_SLOTS]);

    // Introduce a delay to allow multiple passengers to enter the queue
    sleep(2);

    // Czekanie w kolejce na swoją kolej
    int inQueue = 1; // flaga, czy pasażer jest w kolejce
    while (inQueue) {
        sem_wait(&semaphores[MAX_SLOTS]); // Dostęp do kolejki
        struct passenger_entry *entry;
        TAILQ_FOREACH(entry, &sharedData->queue, entries) {
            if (entry->passengerID == passengerID) {
                // Pasażer jest na początku kolejki
                if (TAILQ_FIRST(&sharedData->queue) == entry) {
                    TAILQ_REMOVE(&sharedData->queue, entry, entries);
                    free(entry);
                    inQueue = 0;
                    break;
                }
            }
        }
        sem_post(&semaphores[MAX_SLOTS]);
        if (inQueue) {
            sleep(1); // Czekanie na swoją kolej
        }
    }

    // Wait for stairs capacity
    sem_wait(&sharedData->stairsSemaphore);
    sharedData->passengersOnStairs++;
    printf("Pasażer %d: wchodzi na schody. Liczba pasażerów na schodach: %d\n", passengerID, sharedData->passengersOnStairs);

    // Simulate time on stairs
    sleep(1);

    // Enter the plane
    sharedData->passengersOnStairs--;
    sharedData->passengersInPlane++;
    printf("Pasażer %d: wchodzi do samolotu. Liczba pasażerów w samolocie: %d\n", passengerID, sharedData->passengersInPlane);

    // Release stairs capacity
    sem_post(&sharedData->stairsSemaphore);

    shmdt(sharedData);
}

int verify_baggage(int weight, int isVip) {
    int weight_limit = isVip ? 30 : 20; // wyższy limit bagażu dla VIPa
    if (weight > weight_limit) {
        printf("Pasażer: Bagaż o wadze %d kg przekracza limit %d kg.\n", weight, weight_limit);
        return 0;
    }
    printf("Pasażer: Bagaż o wadze %d kg zaakceptowany.\n", weight);
    return 1;
}