#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

// Stałe konfuguracyjne dla symulacji
#define MAX_PASSENGERS 100
#define MAX_VIP_PASSENERS 10
#define MAX_WEIGHT 20
#define MAX_SLOTS 3
#define MAX_STAIR_CAPACITY 5

// Semafory do synchhronizacji procesów
extern sem_t controlStations[MAX_SLOTS];
extern sem_t stairs;
extern sem_t planeSeats;
extern volatile bool terminateSimulation;

// Prototypy funkcji globalnych
void initialize_sync();
void cleanup_sync();
void singal_handler(int sig);

#endif