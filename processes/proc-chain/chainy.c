#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

enum {
  MAX_ARGS_COUNT = 256,
  MAX_CHAIN_LINKS_COUNT = 256
};

typedef struct {
  char* command;
  uint64_t argc;
  char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
  uint64_t chain_links_count;
  chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void create_chain(char* command, chain_t* chain) {
  char* saveptr = NULL;
  char* token = strtok_r(command, "|", &saveptr);

  chain->chain_links_count = 0;

  while (token != NULL) {
    if (chain->chain_links_count >= MAX_CHAIN_LINKS_COUNT) {
      fprintf(stderr, "Too many commands in pipeline\n");
      exit(EXIT_FAILURE);
    }

    chain_link_t* link = &chain->chain_links[chain->chain_links_count++];
    link->argc = 0;

    char* arg_saveptr = NULL;
    char* arg = strtok_r(token, " ", &arg_saveptr);

    link->command = arg;

    while (arg != NULL) {
      if (link->argc >= MAX_ARGS_COUNT) {
        fprintf(stderr, "Too many arguments for command\n");
        exit(EXIT_FAILURE);
      }
      link->argv[link->argc++] = arg;
      arg = strtok_r(NULL, " ", &arg_saveptr);
    }
    link->argv[link->argc] = NULL;

    token = strtok_r(NULL, "|", &saveptr);
  }
}

void run_chain(chain_t* chain) {
  pid_t pids[MAX_CHAIN_LINKS_COUNT] = {};
  int prev_pipe[2] = {-1, -1};
  int curr_pipe[2] = {-1, -1};
  for (uint64_t i = 0; i < chain->chain_links_count; i++) {
    if (pipe(curr_pipe) == -1) {
      exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == 0) {
      if (i > 0) {
        dup2(prev_pipe[0], STDIN_FILENO);
        close(prev_pipe[1]);
      }
      if (i < chain->chain_links_count - 1) {
        dup2(curr_pipe[1], STDOUT_FILENO);
        close(curr_pipe[0]);
      }
      execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
      perror("execvp");
      exit(EXIT_FAILURE);
    } else {
      if (i > 0) {
        close(prev_pipe[0]);
        close(prev_pipe[1]);
      }
      prev_pipe[0] = curr_pipe[0];
      prev_pipe[1] = curr_pipe[1];
      pids[i] = pid;
    }
  }

  close(curr_pipe[0]);
  close(curr_pipe[1]);

  for (uint64_t i = 0; i < chain->chain_links_count; i++) {
    waitpid(pids[i], NULL, 0);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s \"command1 | command2 | ...\"\n", argv[0]);
    return EXIT_FAILURE;
  }

  chain_t chain;
  create_chain(argv[1], &chain);
  run_chain(&chain);

  return EXIT_SUCCESS;
}
