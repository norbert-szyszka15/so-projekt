#include <assert.h>
#include "common.h"

void test_queue() {
    Queue queue;
    initialize_queue(&queue);

    // Kolejka początkowo jest pusta
    assert(is_queue_empty(&queue) == true);

    // Dodanie elementów do kolejki
    assert(enqueue(&queue, 1) == 1);
    assert(enqueue(&queue, 2) == 1);

    // Pobieranie elementów z kolejki
    assert(dequeue(&queue) == 1);
    assert(dequeue(&queue) == 2);

    // Kolejka powinna być pusta po usunięciu elementów
    assert(is_queue_empty(&queue) == true);

    printf("Wszystkie testy zakończone sukcesem!\n");
}

int main() {
    test_queue();
    return 0;
}
