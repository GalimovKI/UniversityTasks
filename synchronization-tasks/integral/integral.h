#pragma once

#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wait.h"

#define EPSILON 1e-8

typedef double field_t;

typedef field_t func_t(field_t);

field_t integrate(func_t *f, field_t a, field_t b) {
  int n = 1;
  field_t ans = 0.0;
  field_t prev_ans = 0.0;
  field_t delta = (b - a) / n;
  while (fabs(ans - prev_ans) >= EPSILON || n == 1) {
    prev_ans = ans;
    ans = 0.0;
    for (int i = 0; i < n; ++i) {
      ans += delta / 2.0 * (f(a + i * delta) + f(a + (i + 1) * delta));
    }
    delta /= 2;
    n *= 2;
  }
  return ans;
}

enum integrator_state { STATE_IDLE,
                        STATE_RUNNING,
                        STATE_TERMINATE,
                        STATE_ANS
};

//
// QUEUE_PART_START
//

typedef struct task {
  func_t *func;
  field_t a;
  field_t b;
} task_t;

typedef struct taskNode {
  task_t *task;
  struct taskNode *next;
} taskNode;

typedef struct taskQueue {
  taskNode *front;
  taskNode *back;
  size_t size;
} taskQueue;

taskQueue *create_taskQueue() {
  taskQueue *queue = (taskQueue *)malloc(sizeof(taskQueue));
  queue->front = NULL;
  queue->back = NULL;
  queue->size = 0;
  return queue;
}

void task_enqueue(taskQueue *queue, field_t a, field_t b, func_t *func) {
  taskNode *newNode = (taskNode *)malloc(sizeof(taskNode));
  newNode->task = (task_t *)malloc(sizeof(task_t));
  newNode->task->a = a;
  newNode->task->b = b;
  newNode->task->func = func;
  newNode->next = NULL;
  if (queue->back) {
    queue->back->next = newNode;
  }
  queue->back = newNode;
  if (!queue->front) {
    queue->front = newNode;
  }
  queue->size++;
}

task_t *task_dequeue(taskQueue *queue) {
  taskNode *temp = queue->front;
  task_t *value = temp->task;
  queue->front = queue->front->next;
  if (!queue->front) {
    queue->back = NULL;
  }
  queue->size--;
  free(temp);
  return value;
}

u_int32_t empty_taskNode(taskQueue *queue) {
  return queue->size == 0;
}

typedef struct ansNode {
  field_t ans;
  struct ansNode *next;
} ansNode;

typedef struct ansQueue {
  ansNode *front;
  ansNode *back;
  size_t size;
} ansQueue;

ansQueue *create_ansQueue() {
  ansQueue *queue = (ansQueue *)malloc(sizeof(ansQueue));
  queue->front = NULL;
  queue->back = NULL;
  queue->size = 0;
  return queue;
}

void ans_enqueue(ansQueue *queue, field_t ans) {
  ansNode *newNode = (ansNode *)malloc(sizeof(ansNode));
  newNode->ans = ans;
  newNode->next = NULL;
  if (queue->back) {
    queue->back->next = newNode;
  }
  queue->back = newNode;
  if (!queue->front) {
    queue->front = newNode;
  }
  queue->size++;
}

field_t ans_dequeue(ansQueue *queue) {
  ansNode *temp = queue->front;
  field_t value = temp->ans;
  queue->front = queue->front->next;
  if (!queue->front) {
    queue->back = NULL;
  }
  free(temp);
  queue->size--;
  return value;
}

u_int32_t empty_ansNode(ansQueue *queue) {
  return queue->size == 0;
}

void destroy_ansQueue(ansQueue *queue) {
  free(queue);
}

void destroy_taskQueue(taskQueue *queue) {
  free(queue);
}

//
// QUEUE_PART_END
//

typedef struct thread_info {
  taskQueue *tq;
  ansQueue *aq;
  _Atomic(uint32_t) curr_state;
} thread_info_t;

typedef struct par_integrator {
  size_t threads_num;
  pthread_t *threads;
  thread_info_t *thread_info;
  ansQueue *final_ans;
  _Atomic(uint32_t) curr_ans_state;
} par_integrator_t;

void *thread_routine(void *arg);

int par_integrator_init(par_integrator_t *self, size_t threads_num) {
  self->threads_num = threads_num;
  self->threads = (pthread_t *)calloc(threads_num, sizeof(pthread_t));
  self->thread_info = (thread_info_t *)calloc(threads_num, sizeof(thread_info_t));
  for (int i = 0; i < threads_num; ++i) {
    self->thread_info[i].tq = create_taskQueue();
    self->thread_info[i].aq = create_ansQueue();
  }
  self->final_ans = create_ansQueue();
  self->curr_ans_state = STATE_IDLE;
  for (int i = 0; i < threads_num; ++i) {
    pthread_create(&self->threads[i], NULL, thread_routine, &self->thread_info[i]);
  }
  return 0;
}

void *thread_routine(void *arg) {
  thread_info_t *info = (thread_info_t *)arg;
  while (1) {
    atomic_wait(&info->curr_state, STATE_IDLE);
    if (info->curr_state == STATE_TERMINATE) {
      break;
    }
    task_t *value = task_dequeue(info->tq);
    ans_enqueue(info->aq, integrate(value->func, value->a, value->b));
    free(value);
    info->curr_state = STATE_IDLE;
    atomic_notify_one(&info->curr_state);
  }
}

int par_integrator_start_calc(par_integrator_t *self, func_t *func,
                              field_t left_bound, field_t right_bound) {
  field_t delta = (right_bound - left_bound) / self->threads_num;
  for (int i = 0; i < self->threads_num; ++i) {
    task_enqueue(self->thread_info[i].tq, left_bound + i * delta, left_bound + (i + 1) * delta, func);
    atomic_wait(&self->thread_info[i].curr_state, STATE_RUNNING);
    self->thread_info[i].curr_state = STATE_RUNNING;
    atomic_notify_one(&self->thread_info[i].curr_state);
  }
  field_t fans = 0.0;
  for (int i = 0; i < self->threads_num; ++i) {
    atomic_wait(&self->thread_info[i].curr_state, STATE_RUNNING);
    fans += ans_dequeue(self->thread_info[i].aq);
  }
  ans_enqueue(self->final_ans, fans);
  self->curr_ans_state = STATE_ANS;
  atomic_notify_one(&self->curr_ans_state);
  return 0;
}

int par_integrator_get_result(par_integrator_t *self, field_t *result) {
  atomic_wait(&self->curr_ans_state, STATE_IDLE);
  *result = ans_dequeue(self->final_ans);
  return 0;
}

int par_integrator_destroy(par_integrator_t *self) {
  for (int i = 0; i < self->threads_num; ++i) {
    self->thread_info[i].curr_state = STATE_TERMINATE;
    atomic_notify_all(&self->thread_info[i].curr_state);
    pthread_join(self->threads[i], NULL);
    destroy_ansQueue(self->thread_info[i].aq);
    destroy_taskQueue(self->thread_info[i].tq);
  }
  destroy_ansQueue(self->final_ans);
  free(self->threads);
  free(self->thread_info);
  self->curr_ans_state = 0;
  self->threads_num = 0;
  return 0;
}
