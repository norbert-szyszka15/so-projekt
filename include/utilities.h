#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
    int elements[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} Queue;

// Prototypy operacji na kolejce
void initialize_queue(Queue* queue);
int enqueue(Queue* queue, int value);
int dequeue(Queue* queue);
int is_queue_empty(Queue* queue);

// Prototypy fukcji pomocniczych
void log_message(const char* format, ...);
int generate_random_weight();
int generate_random_gender();

#endif