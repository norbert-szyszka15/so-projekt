#include "common.h"
#include "dispatcher.h"

#include <stdio.h>
#include <unistd.h>

void* dispatcher_routine(void* arg) {
    while (!terminateSimulation) {
        sleep(10);
        printf("Dyspozytor: sygnał wcześniejszego odlotu.\n");
    }
    return NULL;
}
