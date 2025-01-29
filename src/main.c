#include "common.h"
#include "passenger.h"
#include "captain.h"
#include "dispatcher.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

int shmID;
SharedData* sharedData;
sem_t semaphores[MAX_SLOTS + 1];

void signal_handler(int sig);
void initialize_resources();
void cleanup_resources(int shmID, SharedData* sharedDataArg);

int main() {
    // Obsługa sygnałów
    signal(SIGINT, signal_handler);

    // Inicjalizacja zasobów
    initialize_resources();

    // Tworzenie procesów dla pasażerów
    for (int i = 0; i < MAX_PASSENGERS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            passenger_process(shmID, semaphores, i + 1);
            exit(0);
        }
        // Add a small delay between creating passenger processes
        usleep(100000); // 100 ms
    }

    // Tworzenie procesów dla kapitanów
    pid_t captainPids[NUM_GATES];
    for (int i = 0; i < NUM_GATES; i++) {
        captainPids[i] = fork();
        if (captainPids[i] == 0) {
            captain_process(shmID, semaphores, i);
            exit(0);
        }
    }

    // Tworzenie procesu dla dyspozytora
    pid_t dispatcherPid = fork();
    if (dispatcherPid == 0) {
        dispatcher_process(shmID, semaphores);
        exit(0);
    }

    // Oczekiwanie na zakończenie symulacji
    while (!sharedData->terminateSimulation) {
        sleep(1);
    }

    // Poczekaj na zakończenie wszystkich procesów
    int status;
    while (wait(&status) > 0);

    // Czyszczenie zasobów
    cleanup_resources(shmID, sharedData);

    printf("Symulacja zakończona.\n");
    return 0;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Zakończenie symulacji z powodu sygnału.\n");
        sharedData->terminateSimulation = true; // Ustawienie flagi w pamięci współdzielonej

        // Send termination signal to all child processes
        kill(0, SIGTERM);

        // Poczekaj na zakończenie wszystkich procesów
        int status;
        while (wait(&status) > 0);

        cleanup_resources(shmID, sharedData);
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
    for (int i = 0; i < NUM_GATES; i++) {
        sharedData->passengersInPlanes[i] = 0;
    }
    TAILQ_INIT(&sharedData->queue);

    // Inicjalizacja semaforów
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (sem_init(&semaphores[i], 1, 2) == -1) { // Maks. 2 osoby na stanowisku
            perror("sem_init");
            exit(1);
        }
    }
    if (sem_init(&sharedData->stairsSemaphore, 1, MAX_STAIRS_CAPACITY) == -1) {
        perror("sem_init");
        exit(1);
    }

    // Inicjalizacja semafora dla kolejki
    if (sem_init(&semaphores[MAX_SLOTS], 1, 1) == -1) {
        perror("sem_init (kolejka)");
        exit(1);
    }
}

void cleanup_resources(int shmID, SharedData* sharedDataArg) {
    // Odłączenie pamięci współdzielonej
    shmdt(sharedDataArg);

    // Usuń pamięć współdzieloną
    if (shmctl(shmID, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    } else {
        printf("Pamięć współdzielona usunięta\n");
    }

    // Zniszczenie semaforów
    for (int i = 0; i < MAX_SLOTS + 1; i++) {
        if (sem_destroy(&semaphores[i]) == -1) {
            perror("sem_destroy");
        }
    }
    if (sem_destroy(&sharedDataArg->stairsSemaphore) == -1) {
        perror("sem_destroy");
    }
}