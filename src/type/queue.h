#ifndef type_queue_h
#define type_queue_h

#include <stddef.h>

typedef struct Queue Queue;

Queue *queue_create();

void queue_free(Queue *queue);

size_t queue_size(Queue *queue);

void queue_enqueue(Queue *queue, void *value);

void *queue_dequeue(Queue *queue);

#endif
