#include "common.h"
#include "captain.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/msg.h>

void captain_process(int shmID, sem_t* semaphores, int gateID) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    // Otwórz kolejkę wiadomości
    int msgQueueID = msgget(KEY_MSG_QUEUE, 0666);
    if (msgQueueID == -1) {
        perror("msgget");
        shmdt(sharedData);
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        // Sprawdź, czy samolot jest pełny i wyślij prośbę o wcześniejszy odlot
        if (sharedData->passengersInPlanes[gateID] >= PLANE_CAPACITY) {
            printf("Kapitan %d: samolot pełny, wysyłanie prośby o wcześniejszy odlot.\n", gateID + 1);

            // Wyślij wiadomość do dyspozytora
            Message msg;
            msg.mtype = gateID + 1; // Unikalny typ wiadomości dla każdej bramy
            msg.gateID = gateID;
            if (msgsnd(msgQueueID, &msg, sizeof(msg.gateID), 0) == -1) {
                perror("msgsnd");
                shmdt(sharedData);
                exit(1);
            }

            // Oczekiwanie na zatwierdzenie
            printf("Kapitan %d: oczekiwanie na zatwierdzenie wcześniejszego odlotu.\n", gateID + 1);
            if (msgrcv(msgQueueID, &msg, sizeof(msg.gateID), gateID + 1 + NUM_GATES, 0) == -1) { // Unikalny typ wiadomości dla zatwierdzenia
                perror("msgrcv");
                shmdt(sharedData);
                exit(1);
            }
            printf("Kapitan %d: wcześniejszy odlot zatwierdzony.\n", gateID + 1);

            // Odlot natychmiast po zatwierdzeniu
            printf("Kapitan %d: samolot startuje z %d pasażerami.\n", gateID + 1, sharedData->passengersInPlanes[gateID]);

            // Lot i powrót
            sleep(3);
            printf("Kapitan %d: samolot wylądował.\n", gateID + 1);

            // Resetowanie liczby pasażerów na pokładzie
            sharedData->passengersInPlanes[gateID] = 0;
            continue; // Pomiń okres oczekiwania i sprawdź nowych pasażerów
        }

        // Oczekiwanie na załadunek pasażerów
        sleep(30 * (gateID + 1));
        printf("Kapitan %d: samolot gotowy do startu.\n", gateID + 1);

        // Sprawdzenie, czy wszyscy pasażerowie są na pokładzie
        if (sharedData->passengersInPlanes[gateID] > 0) {
            printf("Kapitan %d: samolot startuje z %d pasażerami.\n", gateID + 1, sharedData->passengersInPlanes[gateID]);

            // Lot i powrót
            sleep(3);
            printf("Kapitan %d: samolot wylądował.\n", gateID + 1);

            // Resetowanie liczby pasażerów na pokładzie
            sharedData->passengersInPlanes[gateID] = 0;
        } else {
            printf("Kapitan %d: brak pasażerów na pokładzie, odlot odroczony.\n", gateID + 1);
        }
    }

    shmdt(sharedData);
    exit(0); // Upewnij się, że proces kończy się poprawnie
}