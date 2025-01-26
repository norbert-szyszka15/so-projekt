#include "common.h"
#include "passenger.h"

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

    // Symulacja odprawy
    if (rand() % 2 == 0) { // Symulacja losowej wagi bagażu
        printf("Pasażer %d: bagaż przekracza limit, odrzucony.\n", passengerID);
        shmdt(sharedData);
        return;
    }

    printf("Pasażer %d: czeka na kontrolę bezpieczeństwa.\n", passengerID);

    // Kontrola bezpieczeństwa
    semaphore_wait(semID, passengerID % MAX_SLOTS); // Przypisanie do stanowiska
    printf("Pasażer %d: przeszedł kontrolę na stanowisku %d.\n", passengerID, passengerID % MAX_SLOTS);
    sleep(1); // Czas kontroli
    semaphore_signal(semID, passengerID % MAX_SLOTS);

    // Wejście na schody
    semaphore_wait(semID, MAX_SLOTS);
    printf("Pasażer %d: wchodzi na schody.\n", passengerID);
    sleep(1); // Symulacja wchodzenia po schodach
    semaphore_signal(semID, MAX_SLOTS);

    // Wejście na pokład
    semaphore_wait(semID, MAX_SLOTS + 1);
    printf("Pasażer %d: zajmuje miejsce w samolocie.\n", passengerID);

    // Aktualizacja danych współdzielonych
    sharedData->passengersInPlane++;
    shmdt(sharedData);
}
