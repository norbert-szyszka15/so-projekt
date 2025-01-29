#ifndef COMMON_H
#define COMMON_H

#include "utilities.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/queue.h>
#include <sys/msg.h>

// Klucze IPC
#define KEY_SHARED_MEM 0x5678
#define KEY_MSG_QUEUE 0x1234

// Stałe konfuguracyjne dla symulacji
#define MAX_PASSENGERS 1000
#define MAX_VIP_PASSENGERS 10
#define MAX_WEIGHT 20
#define MAX_SLOTS 3
#define MAX_STAIRS_CAPACITY 5
#define NUM_GATES 10
#define PLANE_CAPACITY 20

// Struktura dla elementów kolejki
struct passenger_entry {
    int passengerID;
    int frustrationLevel; // Poziom frustracji pasażera
    TAILQ_ENTRY(passenger_entry) entries;
};

// Struktura daych pamięci współdzielonej
TAILQ_HEAD(passenger_queue, passenger_entry);

typedef struct {
    int passengersInQueue;
    int passengersOnStairs;
    int passengersInPlane;
    bool terminateSimulation;
    int currentGender[MAX_SLOTS]; // -1: puste, 0: mężczyźni, 1: kobiety
    struct passenger_queue queue; // kolejka pasażerów FIFO
    sem_t stairsSemaphore; // Semafor dla schodów
    int passengersInPlanes[NUM_GATES]; // Liczba pasażerów w samolotach na każdym gate
} SharedData;

// Struktura wiadomości
typedef struct {
    long mtype;
    int gateID;
} Message;


int check_gender_and_set(int* currentGender, int slot, int gender);
void reset_gender(int* currentGender, int slot);

#endif