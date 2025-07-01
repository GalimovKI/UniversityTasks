#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct Counter {
  char filename[PATH_MAX];
  int counter;
  struct Counter* next;
} Counter;

typedef struct Counters {
  struct Counter* head;
} Counters;

void increment(Counters* counters, char* filename, int value) {
  Counter* current = counters->head;
  while (current != NULL) {
    if (strncmp(current->filename, filename, PATH_MAX) == 0) {
      current->counter += value;
      return;
    }
    current = current->next;
  }
  Counter* new_head = malloc(sizeof(Counter));
  new_head->next = counters->head;
  new_head->counter = value;
  strncpy(new_head->filename, filename, PATH_MAX - 1);
  counters->head = new_head;
}

void print(Counters* counters) {
  Counter* current = counters->head;
  while (current != NULL) {
    printf("%s:%d\n", current->filename, current->counter);
    current = current->next;
  }
}

void create_file_path(pid_t pid, unsigned long long fd, char* file_path) {
  char symlink[PATH_MAX] = {};

  snprintf(symlink, PATH_MAX, "/proc/%d/fd/%llu", pid,
           fd);
  ssize_t path_size =
      readlink(symlink, file_path, PATH_MAX);

  file_path[path_size] = '\0';
}

int main(int argc, char* argv[]) {
  Counters* counters = malloc(sizeof(Counter));
  counters->head = NULL;

  pid_t pid = fork();

  if (pid == 0) {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);

    execvp(argv[1], &argv[1]);

    exit(EXIT_FAILURE);
  }

  int status = 0;

  waitpid(pid, &status, 0);

  while (!WIFEXITED(status)) {
    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
    waitpid(pid, &status, 0);

    if (WIFSTOPPED(status)) {
      struct user_regs_struct regs;
      ptrace(PTRACE_GETREGS, pid, NULL, &regs);

      if (regs.orig_rax == SYS_write) {
        char file_path[PATH_MAX] = {};
        create_file_path(pid, regs.rdi, file_path);
        increment(counters, file_path, regs.rdx);
      }

      ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
      waitpid(pid, &status, 0);
    }
  }

  print(counters);
}