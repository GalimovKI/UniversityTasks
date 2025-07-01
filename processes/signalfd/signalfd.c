#define _GNU_SOURCE

#include "signalfd.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int signal_pipe[2];

static void signal_handler(int signo) {
  write(signal_pipe[1], &signo, sizeof(signo));
}

int signalfd() {
  if (pipe(signal_pipe) == -1) {
    return -1;
  }

  fcntl(signal_pipe[0], F_SETFL, O_NONBLOCK);

  struct sigaction sigact;
  memset(&sigact, 0, sizeof(sigact));
  sigact.sa_handler = signal_handler;

  for (int sig = 1; sig < 32; ++sig) {
    if (sig == SIGKILL || sig == SIGSTOP) {
      continue;
    }
    if (sigaction(sig, &sigact, NULL) == -1) {
      return -1;
    }
  }

  return signal_pipe[0];
}
