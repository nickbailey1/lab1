#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Must provide at least one argument.");
    exit(EXIT_FAILURE);
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
  for (int i = 0; i < argc-1; i++) {
    wait(NULL);
  }
  return 0;
	

  /*
  // implementing single-pipe version first for practice

  int pipefd[2];

  // check arg count, should be 3 because we only support two piped programs
  if (argc != 3) {
    fprintf(stderr, "Can only pipe two programs for now...");
    exit(EXIT_FAILURE);
  }
  // create pipe, exit if fails
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  // fork first child
  pid_t pid1 = fork();
  if (pid1 == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  // child1 process
  if (pid1 == 0) {
    close(pipefd[0]); // close unused read end of pipe
    dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to be the pipe write end
    close(pipefd[1]); // pipefd[1] is now unused so close it

    execlp(argv[1], argv[1], NULL); // call first program and give it the required argument of its own prog name
    perror("execlp");
    exit(EXIT_FAILURE); // execlp only returns if there is an error, so we need to catch error and exit
  }

  pid_t pid2 = fork();
  if (pid2 == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid2 == 0) {
    close(pipefd[1]); // close unused write end of pipe
    dup2(pipefd[0], STDIN_FILENO); // redirect stdin to be the pipe read end
    close(pipefd[0]); // now unused so close it

    execlp(argv[2], argv[2], NULL); // call second program
    perror("execlp");
    exit(EXIT_FAILURE);
  }

  close(pipefd[0]);
  close(pipefd[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  */
  
  return 0;
}
