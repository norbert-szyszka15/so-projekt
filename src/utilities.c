#include "utilities.h"

#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

void log_message(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int generate_random_weight() {
    srand(time(NULL) ^ getpid()); // Seedowanie generatora liczb losowych pidem procesu
    return 5 + rand() % 46; // Generowanie losowej wagi od 5 do 50
}

int generate_random_gender() {
    srand(time(NULL) ^ getpid()); // Seedowanie generatora liczb losowych pidem procesu
    return rand() % 2; // 0 lub 1
}