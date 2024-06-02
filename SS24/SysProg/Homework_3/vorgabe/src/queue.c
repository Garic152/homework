#include "../lib/queue.h"
#include <stdio.h>
#include <stdlib.h>

int queue_add(void *new_object, queue_object *queue) {
  if (new_object == NULL || queue == NULL) {
    return 1;
  }

  queue_object *new_node = malloc(sizeof(queue_object));
  if (!new_node) {
    return 1;
  }

  new_node->object = new_object;
  new_node->next = queue->next;

  queue->next = new_node;

  return 0;
}

void *queue_poll(queue_object *queue) {
  if (queue->next == NULL) {
    return NULL;
  }

  queue_object *current = queue;

  while (current->next->next != NULL) {
    current = current->next;
  }

  free(current->next);
  current->next = NULL;

  return queue->next->object;
}

queue_object *new_queue() {
  queue_object *queue = malloc(sizeof(queue_object));
  if (!queue) {
    return NULL;
  }

  queue->object = NULL;
  queue->next = NULL;

  return queue;
}

void free_queue(queue_object *queue) {
  queue_object *current = queue;

  while (current != NULL) {
    queue_object *next = current->next;
    free(current);
    current = next;
  }
}

void *queue_peek(queue_object *queue) {
  queue_object *current = queue;

  while (current->next != NULL) {
    current = current->next;
  }
  return current->object;
}
