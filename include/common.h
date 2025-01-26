#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdbool.h>

// Klucze IPC
#define KEY_SEMAPHORE 0x1234
#define KEY_SHARED_MEM 0x5678

// Stałe konfuguracyjne dla symulacji
#define MAX_PASSENGERS 100
#define MAX_VIP_PASSENGERS 10
#define MAX_WEIGHT 20
#define MAX_SLOTS 3
#define MAX_STAIR_CAPACITY 5

// Struktura daych pamięci współdzielonej
typedef struct {
    int passengersInQueue;
    int passengersOnStairs;
    int passengersInPlane;
    bool terminateSimulation;
} SharedData;

// Prototypy operacji semaforowych
void semaphore_wait(int semID, int semNum);
void semaphore_signal(int semID, int semNum);

#endif