#include "common.h"
#include "passenger.h"
#include "captain.h"
#include "dispatcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int shmID, semID;
SharedData* sharedData;

void signal_handler(int sig);
void initialize_resources();
void cleanup_resources(int shmID, SharedData* sharedDataArg, int semID);

int main() {
    // Obsługa sygnałów
    signal(SIGINT, signal_handler);

    // Inicjalizacja zasobów
    initialize_resources();

    // Tworzenie procesów dla pasażerów
    for (int i = 0; i < MAX_PASSENGERS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            passenger_process(shmID, semID, i + 1);
            exit(0);
        }
    }

    // Tworzenie procesu dla kapitana
    pid_t captainPid = fork();
    if (captainPid == 0) {
        captain_process(shmID, semID);
        exit(0);
    }

    // Tworzenie procesu dla dyspozytora
    pid_t dispatcherPid = fork();
    if (dispatcherPid == 0) {
        dispatcher_process(shmID, semID);
        exit(0);
    }

    // Oczekiwanie na zakończenie symulacji
    while (!sharedData->terminateSimulation) {
        sleep(1);
    }

    // Czyszczenie zasobów
    cleanup_resources(shmID, sharedData, semID);

    printf("Symulacja zakończona.\n");
    return 0;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Zakończenie symulacji z powodu sygnału.\n");
        sharedData->terminateSimulation = true; // Ustawienie flagi w pamięci współdzielonej
        cleanup_resources(shmID, sharedData, semID);
        exit(0);
    }
}

void initialize_resources() {
    // Tworzenie pamięci współdzielonej
    shmID = shmget(KEY_SHARED_MEM, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmID == -1) {
        perror("shmget");
        exit(1);
    }
    sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    // Inicjalizacja danych w pamięci współdzielonej
    sharedData->passengersInQueue = 0;
    sharedData->passengersOnStairs = 0;
    sharedData->passengersInPlane = 0;
    sharedData->terminateSimulation = false;
    for (int i = 0; i < MAX_SLOTS; i++) {
        sharedData->currentGender[i] = -1; // Stanowisko puste
    }
    initialize_queue(&sharedData->queue);

    // Tworzenie semaforów
    semID = semget(KEY_SEMAPHORE, MAX_SLOTS + 2, IPC_CREAT | 0666);
    if (semID == -1) {
        perror("semget");
        exit(1);
    }

    // Inicjalizacja semaforów dla stanowisk (MAX_SLOTS = 3)
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (semctl(semID, i, SETVAL, 2) == -1) { // Maks. 2 osoby na stanowisku
            perror("semctl");
            exit(1);
        }
    }

    // Inicjalizacja semafora dla kolejki
    if (semctl(semID, MAX_SLOTS + 1, SETVAL, 1) == -1) {
        perror("semctl (kolejka)");
        exit(1);
    }

    // Inicjalizacja semaforów dla schodów i samolotu
    if (semctl(semID, MAX_SLOTS, SETVAL, MAX_STAIR_CAPACITY) == -1) {
        perror("semctl");
        exit(1);
    }
    if (semctl(semID, MAX_SLOTS + 1, SETVAL, MAX_PASSENGERS) == -1) {
        perror("semctl");
        exit(1);
    }
}

void cleanup_resources(int shmID, SharedData* sharedDataArg, int semID) {
    // Odłączenie pamięci współdzielonej
    shmdt(sharedDataArg);

    // Poczekaj na zakończenie wszystkich procesów
    int status;
    while (wait(&status) > 0);

    // Usuń pamięć współdzieloną i semafory
    if (shmctl(shmID, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    } else {
        printf("Pamięć współdzielona usunięta\n");
    }
    if (semctl(semID, 0, IPC_RMID) == -1) {
        perror("semctl");
    } else {
        printf("Semafory usunięte\n");
    }
}