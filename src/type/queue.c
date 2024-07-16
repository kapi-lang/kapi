#include <pthread.h>

#include "macros.h"
#include "type/queue.h"

typedef struct QueueNode {
  struct QueueNode *next;
  struct QueueNode *previous;
  void *data;
} QueueNode;

struct Queue {
  QueueNode *head;
  QueueNode *tail;
  size_t size;
  pthread_mutex_t mutex;
};

Queue *queue_create() {
  Queue *queue = NEW(Queue);

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;

  pthread_mutex_init(&queue->mutex, NULL);

  return queue;
}

void queue_free(Queue *queue) {
  void *data = NULL;
  while ((data = queue_dequeue(queue))) {
    free(data);
  }
  pthread_mutex_destroy(&queue->mutex);
  free(queue);
}

size_t queue_size(Queue *queue) {
  pthread_mutex_lock(&queue->mutex);
  size_t size = queue->size;
  pthread_mutex_unlock(&queue->mutex);

  return size;
}

void queue_enqueue(Queue *queue, void *value) {
  pthread_mutex_lock(&queue->mutex);
  {
    QueueNode *node = NEW(QueueNode);

    node->data = value;

    if (queue->size == 0) {
      queue->head = node;
      queue->tail = node;
    } else {
      node->previous = queue->tail;
      queue->tail->next = node;
      queue->tail = node;
    }

    queue->size++;
  }
  pthread_mutex_unlock(&queue->mutex);
}

void *queue_dequeue(Queue *queue) {
  if (queue_size(queue) == 0) {
    return NULL;
  }

  void *data = NULL;

  pthread_mutex_lock(&queue->mutex);
  {
    QueueNode *node = queue->head;
    data = node->data;

    if (node->next != NULL) {
      queue->head = node->next;
    }

    queue->size--;

    free(node);
  }
  pthread_mutex_unlock(&queue->mutex);

  return data;
}
