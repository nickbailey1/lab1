#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  /*
  int numPipes = argc - 2;
  int pipes[numPipes][2]; // array of pipes needed

  // create all of the necesssary pipes, exit if any pipe creation fails
  for (int i = 0; i < numPipes; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  */

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
  
  return 0;
}
