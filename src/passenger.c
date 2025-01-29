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
        shmdt(sharedData);
        exit(0); // Wyjście poprawne
    }

    // Wejście do kolejki między odprawą bagażową a kontrolą bezpieczeństwa
    // Zaimplementowane przy pomocy kolejki FIFO
    sem_wait(&semaphores[MAX_SLOTS]); // Dostęp do kolejki
    struct passenger_entry *new_entry = (struct passenger_entry *)malloc(sizeof(struct passenger_entry));
    if (new_entry == NULL) {
        perror("malloc");
        shmdt(sharedData);
        exit(1);
    }
    new_entry->passengerID = passengerID;
    new_entry->frustrationLevel = 0; // Początkowy poziom frustracji

    if (isVip) {
        struct passenger_entry *first_entry = TAILQ_FIRST(&sharedData->queue);
        if (first_entry != NULL) {
            first_entry->frustrationLevel++;
            printf("Pasażer %d: poziom frustracji zwiększony do %d.\n", first_entry->passengerID, first_entry->frustrationLevel);
            if (first_entry->frustrationLevel >= 3) {
                printf("Pasażer %d: poziom frustracji osiągnął maksimum. Opuścił lotnisko.\n", first_entry->passengerID);
                TAILQ_REMOVE(&sharedData->queue, first_entry, entries);
                free(first_entry);
            }
        }
        TAILQ_INSERT_HEAD(&sharedData->queue, new_entry, entries); // VIPs idą na początek
    } else {
        TAILQ_INSERT_TAIL(&sharedData->queue, new_entry, entries);
    }
    printf("Pasażer %d: dodany do kolejki.\n", passengerID);
    sem_post(&semaphores[MAX_SLOTS]);

    // Wprowadzenie opóźnienia, aby umożliwić wielu pasażerom wejście do kolejki
    sleep(2);

    // Czekanie w kolejce na swoją kolej
    int inQueue = 1; // flaga, czy pasażer jest w kolejce
    while (inQueue && !sharedData->terminateSimulation) {
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

    if (sharedData->terminateSimulation) {
        shmdt(sharedData);
        exit(0); // Wyjście poprawne, jeśli symulacja została zakończona
    }

    // Czekanie na dostępność schodów
    sem_wait(&sharedData->stairsSemaphore);
    sharedData->passengersOnStairs++;
    printf("Pasażer %d: wchodzi na schody. Liczba pasażerów na schodach: %d\n", passengerID, sharedData->passengersOnStairs);

    // Symulacja czasu na schodach
    sleep(1);

    // Znalezienie pierwszego dostępnego samolotu z miejscem
    int gateID = -1;
    while (gateID == -1 && !sharedData->terminateSimulation) {
        for (int i = 0; i < NUM_GATES; i++) {
            if (sharedData->passengersInPlanes[i] < PLANE_CAPACITY) {
                gateID = i;
                break;
            }
        }
        if (gateID == -1) {
            sleep(1); // Czekanie na dostępność samolotu
        }
    }

    if (sharedData->terminateSimulation) {
        sharedData->passengersOnStairs--;
        sem_post(&sharedData->stairsSemaphore);
        shmdt(sharedData);
        exit(0); // Wyjście poprawne, jeśli symulacja została zakończona
    }

    // Wejście do samolotu
    sharedData->passengersOnStairs--;
    sharedData->passengersInPlanes[gateID]++;
    printf("Pasażer %d: wchodzi do samolotu %d. Liczba pasażerów w samolocie: %d\n", passengerID, gateID + 1, sharedData->passengersInPlanes[gateID]);

    // Zwolnienie dostępu do schodów
    sem_post(&sharedData->stairsSemaphore);

    shmdt(sharedData);
    exit(0);
}

int verify_baggage(int weight, int isVip) {
    int weight_limit = isVip ? 30 : 20; // Wyższy limit bagażu dla VIPa
    if (weight > weight_limit) {
        printf("Pasażer: Bagaż o wadze %d kg przekracza limit %d kg.\n", weight, weight_limit);
        return 0;
    }
    printf("Pasażer: Bagaż o wadze %d kg zaakceptowany.\n", weight);
    return 1;
}