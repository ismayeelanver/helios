#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @brief Spawn a new process from a given string.
 * 
 * @details
 * This function will spawn a new process by calling the fork() system call. If
 * the fork() call fails, it will print an error message and retry the call until
 * it succeeds. The fork() system call will return twice. Once for the parent
 * process and once for the child process. In the parent process, it will return
 * the process ID of the child process. In the child process, it will return 0.
 * 
 * If the child process is created successfully, the child process will call the
 * execl() system call to execute a shell (/bin/sh) with the -c argument. The -c
 * argument specifies the command to be executed. The child process will execute
 * the given program string as if it was entered into the shell.
 * 
 * If the execl() call fails, the child process will print an error message and
 * exit with a status of EXIT_FAILURE.
 * 
 * @param program The string to be executed as a shell command.
 */
inline auto spawn(const char* program) {
  do {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork failed");
    } else if (pid == 0) {
      execl("/bin/sh", "sh", "-c", program, nullptr);
      perror("execl failed");
      _exit(EXIT_FAILURE);
    }
  } while (0);
}
