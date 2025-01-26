#include "common.h"
#include "captain.h"

#include <stdio.h>
#include <unistd.h>

void* captain_routine(void* arg) {
    while (!terminateSimulation) {
        sleep(5);
        printf("Kapitan: samolot gotowy do startu.\n");
        sleep(3);
        printf("Kapitan: samolot wylądował.\n");
    }
    return NULL;
}
