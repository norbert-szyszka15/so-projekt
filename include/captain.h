#ifndef CAPTAIN_H
#define CAPTAIN_H

#include <semaphore.h>

void captain_process(int shmID, sem_t* semaphores, int gateID);

#endif