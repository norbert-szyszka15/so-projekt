#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <semaphore.h>

void dispatcher_process(int shmID, sem_t* semaphores);

#endif