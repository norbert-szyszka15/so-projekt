#include "utilities.h"

#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

int generate_random_weight() {
    srand(time(NULL) ^ getpid()); // Seedowanie generatora liczb losowych pidem procesu
    return 5 + rand() % 46; // Generowanie losowej wagi od 5 do 50
}

int generate_random_gender() {
    srand(time(NULL) ^ getpid()); // Seedowanie generatora liczb losowych pidem procesu
    return rand() % 2; // 0 lub 1
}