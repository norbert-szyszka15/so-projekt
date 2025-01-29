#include "common.h"
#include "dispatcher.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/msg.h>

void dispatcher_process(int shmID, sem_t* semaphores) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    // Utwórz kolejkę wiadomości
    int msgQueueID = msgget(KEY_MSG_QUEUE, IPC_CREAT | 0666);
    if (msgQueueID == -1) {
        perror("msgget");
        shmdt(sharedData);
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        sleep(10);
        printf("Dyspozytor: sprawdzanie stanu samolotów.\n");

        // Sprawdź, czy są jacyś pasażerowie czekający lub wchodzący na pokład
        bool passengersWaiting = false;
        for (int i = 0; i < NUM_GATES; i++) {
            if (sharedData->passengersInQueue > 0 || sharedData->passengersInPlanes[i] > 0) {
                passengersWaiting = true;
                break;
            }
        }

        if (!passengersWaiting) {
            printf("Dyspozytor: brak pasażerów, kończenie symulacji.\n");
            sharedData->terminateSimulation = true;

            // Wyślij sygnał zakończenia do wszystkich procesów potomnych
            kill(0, SIGTERM);
        }

        // Obsługa prośby o wcześniejszy odlot
        Message msg;
        if (msgrcv(msgQueueID, &msg, sizeof(msg.gateID), 0, IPC_NOWAIT) != -1) {
            printf("Dyspozytor: zatwierdzanie wcześniejszego odlotu dla bramy %d.\n", msg.gateID + 1);
            msg.mtype = msg.gateID + 1 + NUM_GATES; // Unikalny typ wiadomości dla zatwierdzenia
            if (msgsnd(msgQueueID, &msg, sizeof(msg.gateID), 0) == -1) {
                perror("msgsnd");
                shmdt(sharedData);
                exit(1);
            }
        }
    }

    // Usuń kolejkę wiadomości
    if (msgctl(msgQueueID, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }

    shmdt(sharedData);
    exit(0); // Upewnij się, że proces kończy się poprawnie
}