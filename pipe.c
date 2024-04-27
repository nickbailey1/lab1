#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Must provide at least one argument.");
    exit(EINVAL);
  }

  // only one executable given, so no piping
  if (argc == 2) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child process
      execlp(argv[1], argv[1], NULL);
      perror("execlp"); // execlp only returns if there was an error
      exit(EXIT_FAILURE);
    }
    // parent process
    wait(NULL);
    exit(EXIT_SUCCESS);
  }
  
  int numPipes = argc-2; // need one less pipe than there are arguments
  int pipes[numPipes][2]; // array [pipe][fd's]

  // create pipes
  for (int i = 0; i < numPipes; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < argc-1; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child process
      if (i > 0) { // not first command
	dup2(pipes[i-1][0], STDIN_FILENO); // set this child's stdin to be the read fd from the pipe previous to it
      }
      if (i < argc-2) { // not the last command
	dup2(pipes[i][1], STDOUT_FILENO); // set the STDOUT to be the write of the this pipe
      }
      for (int j = 0; j < numPipes; j++) {
	// close all pipes, STDOUT and STDIN file descriptors are set up
	close(pipes[j][0]);
	close(pipes[j][1]);
      }
      execlp(argv[i+1], argv[i+1], NULL);
      perror("execlp");
      exit(EXIT_FAILURE);
    }
  }
  for (int i = 0; i < numPipes; i++) { // close all pipes
    close(pipes[i][0]);
    close(pipes[i][1]);
  }
  // handle final exit code
  int status;
  int exit_status = EXIT_SUCCESS;
  for (int i = 0; i < argc-1; i++) {
    pid_t done = waitpid(-1, &status, 0);
    if (done == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      exit_status = EXIT_FAILURE;
    }
  }

  exit(exit_status);
}
