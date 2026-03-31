#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
  
  int pipefd[2];
  pid_t pid_ls; 
  pid_t pid_sed;

  if(pipe(pipefd) == -1){
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  // premier processus enfant pour ls -l
  pid_ls = fork();
  if(pid_ls == -1){
    perror("fork");
    exit(EXIT_FAILURE);
  } else if(pid_ls == 0){
    // on redirige stdout vers le pipe
    if(dup2(pipefd[1], STDOUT_FILENO) == -1){
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    execlp("ls", "ls", "-l", NULL);
  }

  // deuxième processus enfant pour sed 's/\.c$/.COUCOU/'
  pid_sed = fork();
  if(pid_sed == -1){
    perror("fork");
    exit(EXIT_FAILURE);
  } else if(pid_sed == 0){
    // on redirige stdin depuis le pipe
    if(dup2(pipefd[0], STDIN_FILENO) == -1){
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    close(pipefd[1]);
    close(pipefd[0]);

    execlp("sed", "sed", "s/\\.c$/.COUCOU/", NULL);
  }

  // processus parent
  close(pipefd[0]);
  close(pipefd[1]);

  waitpid(pid_ls, NULL, 0);
  waitpid(pid_sed, NULL, 0);
  
}
















