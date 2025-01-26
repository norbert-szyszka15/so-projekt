#include "utilities.h"

#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

// Funkcja inicjalizująca kolejkę, dodać do main.c w initialize_resources()
// przy inicjalizacji pamięci współdzielonej w odpowiednim miejscu
void initialize_queue(Queue* queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

int enqueue(Queue* queue, int value) {
    if (queue->size == MAX_QUEUE_SIZE) {
        printf("Kolejka jest pełna! Nie można dodać pasażera %d.\n", value);
        return 0;
    }
    queue->elements[queue->rear] = value;

    // printf() do debugowania
    // printf("Pasażer %d dodany na pozycję %d w kolejce.\n", value, queue->rear);
    
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->size++;
    return 1;
}

int dequeue(Queue* queue) {
    if (queue->size == 0) {
        printf("Kolejka jest pusta! Nie można usunąć pasażera.\n");
        return -1;
    }
    int value = queue->elements[queue->front];

    // printf() do debugowania
    // printf("Pasażer %d usunięty z pozycji %d w kolejce.\n", value, queue->front);

    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return value;
}

int is_queue_empty(Queue* queue) {
    return queue->size == 0;
}

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