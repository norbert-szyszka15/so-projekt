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
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        // Check if the plane is full and request early departure
        if (sharedData->passengersInPlanes[gateID] >= PLANE_CAPACITY) {
            printf("Kapitan %d: samolot pełny, wysyłanie prośby o wcześniejszy odlot.\n", gateID + 1);

            // Wyślij wiadomość do dyspozytora
            Message msg;
            msg.mtype = gateID + 1; // Unique message type for each gate
            msg.gateID = gateID;
            if (msgsnd(msgQueueID, &msg, sizeof(msg.gateID), 0) == -1) {
                perror("msgsnd");
                exit(1);
            }

            // Oczekiwanie na zatwierdzenie
            printf("Kapitan %d: oczekiwanie na zatwierdzenie wcześniejszego odlotu.\n", gateID + 1);
            if (msgrcv(msgQueueID, &msg, sizeof(msg.gateID), gateID + 1 + NUM_GATES, 0) == -1) { // Unique message type for approval
                perror("msgrcv");
                exit(1);
            }
            printf("Kapitan %d: wcześniejszy odlot zatwierdzony.\n", gateID + 1);

            // Depart immediately after approval
            printf("Kapitan %d: samolot startuje z %d pasażerami.\n", gateID + 1, sharedData->passengersInPlanes[gateID]);

            // Lot i powrót
            sleep(3);
            printf("Kapitan %d: samolot wylądował.\n", gateID + 1);

            // Resetowanie liczby pasażerów na pokładzie
            sharedData->passengersInPlanes[gateID] = 0;
            continue; // Skip the waiting period and check for new passengers
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
    exit(0); // Ensure the process exits cleanly
}