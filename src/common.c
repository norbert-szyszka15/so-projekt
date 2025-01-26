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

    // printf() do debugowania
    //printf("semaphore_wait: Done waiting on semNum %d\n", semNum);
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

    // printf() do debugowania
    // printf("semaphore_signal: Done signaling on semNum %d\n", semNum);
}

// Funkcja sprawdzająca płeć i ustawiająca ją na stanowisku
int check_gender_and_set(int* currentGender, int slot, int gender) {
    if (currentGender[slot] == -1) {
        // Stanowisko puste, ustawiamy płeć
        currentGender[slot] = gender;
        return 1;
    } else if (currentGender[slot] == gender) {
        // Płeć zgodna z aktualną na stanowisku
        return 1;
    }
    // Płeć inna niż na stanowisku
    return 0;
}

// Funkcja resetująca płeć na stanowisku, gdy jest ono wolne
void reset_gender(int* currentGender, int slot) {
    currentGender[slot] = -1;
}