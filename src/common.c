#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

// Operacja czekania na semaforze
void semaphore_wait(int semID, int semNum) {
    if (semNum < 0 || semNum >= MAX_SLOTS + 2) { // Sprawdzenie zakresu indeksu
        fprintf(stderr, "semaphore_wait: Invalid semNum %d\n", semNum);
        exit(1);
    }

    struct sembuf operation = {(unsigned short)semNum, -1, 0};
    if (semop(semID, &operation, 1) == -1) {
        perror("semaphore_wait");
        exit(1);
    }

    printf("semaphore_wait: Done waiting on semNum %d\n", semNum);
}

// Operacja sygnału na semaforze
void semaphore_signal(int semID, int semNum) {
    if (semNum < 0 || semNum >= MAX_SLOTS + 2) { // Sprawdzenie zakresu indeksu
        fprintf(stderr, "semaphore_signal: Invalid semNum %d\n", semNum);
        exit(1);
    }

    struct sembuf operation = {(unsigned short)semNum, 1, 0};
    if (semop(semID, &operation, 1) == -1) {
        perror("semaphore_signal");
        exit(1);
    }

    printf("semaphore_signal: Done signaling on semNum %d\n", semNum);
}
