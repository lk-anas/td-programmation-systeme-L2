#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>

// ldd: -lcrypto
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>

// = {0}  =>  all the array is reset to zero (only works for zero!)
char hash[1 + 2*MD5_DIGEST_LENGTH] = {0};

char* md5hash (char *str) {
  unsigned char md5[MD5_DIGEST_LENGTH] = {0};
  MD5(str, strlen(str), md5);
  for (int i=0; i<MD5_DIGEST_LENGTH; i++) {
    sprintf(hash + 2*i, "%02x", md5[i]);
  }
  return hash;
}

// vérifie si la chaîne s commence par n caractères '0'
int zeros(char *s, int n) {
  for (int i = 0; i < n; i++) {
    if (s[i] != '0') {
      return 0;
    }
  }
  return 1;
}

// cherche des nonces dont le hash MD5 commence par 'zero' zéros
// et les envoie au parent via le pipe
void bruteforce(int pipe_ecriture, int start, int step, int zero) {
  int nombre = start;
  char temp[7 + 8 * sizeof(int)];
  int trouve;
  while (1) {
    // on convertit le nombre en chaîne pour calculer son hash
    sprintf(temp, "%d", nombre);
    trouve = zeros(md5hash(temp), zero);
    if (trouve == 1) {
      // nonce trouvé : on l'envoie au parent via le pipe
      if (write(pipe_ecriture, &nombre, sizeof(int)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
      }
    }
    // on passe au nombre suivant
    nombre += step;
  }
}

int main() {
  
  // création de 10 pipes : un pipe par enfant
  int pipes[10][2];
  for (int i = 0; i < 10; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  // création de 10 processus enfant
  pid_t pids[10];
  for (int i = 0; i < 10; i++) {
    pids[i] = fork();
    if (pids[i] == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pids[i] == 0) {
      close(pipes[i][0]);
      bruteforce(pipes[i][1], i, 10, 6);
    }
  }

  for (int i = 0; i < 10; i++) {
    close(pipes[i][1]);
  }

  // on prépare le tableau de fiches pour poll()
  // chaque fiche = un pipe a surveiller
  struct pollfd fds[10];
  for (int i = 0; i < 10; i++) {
    fds[i].fd = pipes[i][0];
    fds[i].events = POLLIN;
  }

  // affichage des 5 premiers nonces trouvé
  int nonces = 0;
  while (nonces < 5) {
    // poll() surveille tous les pipes et attend qu'un ait des données
    poll(fds, (nfds_t)10, -1);
    for (int i = 0; i < 10; i++) {
      if (fds[i].revents & POLLIN) {
        int val;
        if (read(fds[i].fd, &val, sizeof(int)) == -1) {
          perror("read");
          exit(EXIT_FAILURE);
        }
        // on affiche le nonce et son hash
        char str_val[7 + 8 * sizeof(int)];
        sprintf(str_val, "%d", val);
        printf("Nonce trouvé : %d\n", val);
        printf("Hash correspondant : %s\n", md5hash(str_val));
        nonces++;
        
        if (nonces >= 5) {
          break;
        }
      }
    }
  }

  // on tue tous les enfants
  for (int i = 0; i < 10; i++) {
    kill(pids[i], SIGKILL);
  }
  
}


















