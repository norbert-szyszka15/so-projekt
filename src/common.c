#include "common.h"

#include <stdio.h>
#include <stdlib.h>

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