//run : bash

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>

bool waiting = false;

void sigInt(int sig) {
  if (waiting == true) {
    printf("\nfin du programme\n");
    exit(EXIT_SUCCESS);
  } else {
    printf("\nappuiez encore sur Ctrl + C dans 2 secondes pour quitter\n");
    waiting = true;
    alarm(2);
  }
}

void sigAlrm(int sig) {
  printf("reprise du travail : \n");
  waiting = false;
}

int main() {
  signal(SIGINT, sigInt);
  signal(SIGALRM, sigAlrm);

  while (1) {
    if (waiting != true) {
      printf("working...\n");
      sleep(1);
    }
  }
}

