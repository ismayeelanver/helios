#include <stdlib.h>
#include <unistd.h>

#define spawn(program)                                                         \
  do {                                                                         \
    pid_t pid = fork();                                                        \
    if (pid == -1) {                                                           \
      perror("fork failed");                                                   \
    } else if (pid == 0) {                                                     \
      execl("/bin/sh", "sh", "-c", program, nullptr);                          \
      perror("execl failed");                                                  \
      _exit(EXIT_FAILURE);                                                     \
    }                                                                          \
  } while (0)
