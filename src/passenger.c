#include "common.h"
#include "passenger.h"
#include "utilities.h"

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
    sem_wait(&semaphores[MAX_SLOTS + 1]); // Dostęp do kolejki
    struct passenger_entry *new_entry = (struct passenger_entry *)malloc(sizeof(struct passenger_entry));
    if (new_entry == NULL) {
        perror("malloc");
        exit(1);
    }
    new_entry->passengerID = passengerID;
    TAILQ_INSERT_TAIL(&sharedData->queue, new_entry, entries);
    printf("Pasażer %d: dodany do kolejki.\n", passengerID);
    sem_post(&semaphores[MAX_SLOTS + 1]);

    // Introduce a delay to allow multiple passengers to enter the queue
    sleep(2);

    // Czekanie w kolejce na swoją kolej
    int inQueue = 1; // flaga, czy pasażer jest w kolejce
    while (inQueue) {
        sem_wait(&semaphores[MAX_SLOTS + 1]); // Dostęp do kolejki
        struct passenger_entry *front_entry = TAILQ_FIRST(&sharedData->queue);
        if (front_entry == NULL) {
            printf("Pasażer %d: kolejka jest pusta!\n", passengerID);
            sem_post(&semaphores[MAX_SLOTS + 1]);
            break;
        }
        printf("Pasażer %d sprawdza kolejkę. Front kolejki: %d.\n", passengerID, front_entry->passengerID);

        if (front_entry->passengerID == passengerID) {
            // Jeśli to kolej tego pasażera, to opuszcza kolejkę
            TAILQ_REMOVE(&sharedData->queue, front_entry, entries);
            free(front_entry);
            printf("Pasażer %d opuszcza kolejkę.\n", passengerID);
            inQueue = 0;

            int slot = passengerID % MAX_SLOTS; // Przydział stanowiska
            int success = 0;
            while (!success) {
                sem_wait(&semaphores[slot]); // Próba zajęcia stanowiska
                success = check_gender_and_set(sharedData->currentGender, slot, gender);
                if (!success) {
                    sem_post(&semaphores[slot]); // Zwolnienie semafora
                    usleep(10000);
                }
            }
            printf("Pasażer %d: przeszedł kontrolę bezpieczeństwa na stanowisku %d.\n", passengerID, slot);
            sleep(1); // Czas kontroli
            sem_post(&semaphores[slot]); // Zwolnienie semafora

            // Reset płci na stanowisku, jeżeli jest ono wolne
            int sval;
            sem_getvalue(&semaphores[slot], &sval);
            if (sval == 2) { // 2 = stanowisko puste
                reset_gender(sharedData->currentGender, slot);
            }
        }

        sem_post(&semaphores[MAX_SLOTS + 1]);
        if (inQueue) {
            sleep(1); // Symulacja czasu spędzonego w kolejce
        }
    }

    // Wejście na schody
    printf("Pasażer %d: wchodzi na schody.\n", passengerID);
    sem_wait(&semaphores[MAX_SLOTS]);
    sleep(1); // Symulacja wchodzenia po schodach
    sem_post(&semaphores[MAX_SLOTS]);

    // Wejście na pokład
    sem_wait(&semaphores[MAX_SLOTS + 1]);
    printf("Pasażer %d: zajmuje miejsce w samolocie.\n", passengerID);

    // Aktualizacja danych współdzielonych
    sharedData->passengersInPlane++;
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