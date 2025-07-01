#pragma once
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

struct timespec ts = {0, 1000};

typedef struct lfstack_node_t {
  uintptr_t value;
  struct lfstack_node_t* next;
} lfstack_node_t;

typedef struct {
  lfstack_node_t* head;
} lfstack_t;

int lfstack_init(lfstack_t* stack) {
  if (!stack) return -1;
  stack->head = NULL;
  return 0;
}

int lfstack_push(lfstack_t* stack, uintptr_t value) {
  if (!stack) return -1;

  lfstack_node_t* new_node = (lfstack_node_t*)malloc(sizeof(lfstack_node_t));
  if (!new_node) return -1;

  new_node->value = value;

  lfstack_node_t* old_head;
  while (1) {
    old_head = atomic_load(&stack->head);
    new_node->next = old_head;
    if (atomic_compare_exchange_weak(&stack->head, &old_head, new_node)) {
      break;
    }
    nanosleep(&ts, NULL);
  }
  return 0;
}

int lfstack_pop(lfstack_t* stack, uintptr_t* value) {
  if (!stack || !value) return 1;

  lfstack_node_t* old_head = NULL;
  while (1) {
    old_head = atomic_load(&stack->head);
    if (!old_head) {
      *value = 0;
      return 0;
    }
    if (atomic_compare_exchange_weak(&stack->head, &old_head, old_head->next)) {
      break;
    }
    nanosleep(&ts, NULL);
  }

  *value = old_head->value;
  free(old_head);
  return 0;
}

int lfstack_destroy(lfstack_t* stack) {
  if (!stack) return -1;
  stack->head = NULL;
  return 0;
}
