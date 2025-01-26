#include "common.h"
#include "passenger.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

void passenger_process(int shmID, int semID, int passengerID) {
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

    // Kontrola bezpieczeństwa
    int slot = passengerID % MAX_SLOTS; // Przydział stanowiska
    int success = 0;
    while (!success) {
        semaphore_wait(semID, slot); // Próba zajęcia stanowiska
        success = check_gender_and_set(sharedData->currentGender, slot, gender);
        if (!success) {
            semaphore_signal(semID, slot); // Zwolnienie semafora
            usleep(10000);
        }
    }
    printf("Pasażer %d: przeszedł kontrolę bezpieczeństwa na stanowisku %d.\n", passengerID, slot);
    sleep(1); // Czas kontroli
    semaphore_signal(semID, slot); // Zwolnienie semafora

    // Reset płci na stanowisku, jeżeli jest ono wolne
    if (semctl(semID, slot, GETVAL) == 2) { // 2 = stanowisko puste
        reset_gender(sharedData->currentGender, slot);
    }

    /*
    printf("Pasażer %d: czeka na kontrolę bezpieczeństwa na stanowisku %d.\n", passengerID, slot);
    semaphore_wait(semID, slot); // Przydział stanowiska (zajęcie semafora)
    printf("Pasażer %d: przeszedł kontrolę bezpieczeństwa na stanowisku %d.\n", passengerID, slot);
    sleep(1); // Czas kontroli
    semaphore_signal(semID, slot); // Zwolnienie semafora
    */

    // Wejście na schody
    printf("Pasażer %d: wchodzi na schody.\n", passengerID);
    semaphore_wait(semID, MAX_SLOTS);
    sleep(1); // Symulacja wchodzenia po schodach
    semaphore_signal(semID, MAX_SLOTS);

    // Wejście na pokład
    semaphore_wait(semID, MAX_SLOTS + 1);
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
