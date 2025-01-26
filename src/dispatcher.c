#include "common.h"
#include "dispatcher.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void dispatcher_process(int shmID, int semID) {
    SharedData* sharedData = (SharedData*)shmat(shmID, NULL, 0);
    if (sharedData == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    while (!sharedData->terminateSimulation) {
        sleep(10);
        printf("Dyspozytor: sygnał dla wcześniejszego odlotu samolotu.\n");
        sharedData->terminateSimulation = true;
    }

    shmdt(sharedData);
}
