#include "lca.h"

#include <stdio.h>
#include <stdlib.h>
#define MAX_PROC_FILE_PATH 1024

pid_t find_parent_pid(pid_t pid) {
  char file_path[MAX_PROC_FILE_PATH] = {};
  snprintf(file_path, MAX_PROC_FILE_PATH, "/proc/%d/stat", pid);
  FILE* proc_stat = fopen(file_path, "r");
  if (!proc_stat) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  pid_t ppid = 0;
  fscanf(proc_stat, "%*d %*s %*c %d", &ppid);
  fclose(proc_stat);

  return ppid;
}

size_t find_path(pid_t pid, pid_t* path) {
  size_t depth = 0;
  do {
    path[depth++] = pid;
    pid = find_parent_pid(pid);
  } while (pid > 1 && depth < MAX_TREE_DEPTH);
  return depth - 1;
}

pid_t find_lca(pid_t x, pid_t y) {
  pid_t first_path[MAX_TREE_DEPTH] = {};
  pid_t second_path[MAX_TREE_DEPTH] = {};
  int i = find_path(x, first_path);
  int j = find_path(y, second_path);
  while (i >= 0 && j >= 0 && first_path[i] == second_path[j]) {
    i--;
    j--;
  }
  return first_path[i + 1];
}
