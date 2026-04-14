// ldd: -lrt
// ldd: -lpthread
#include <pthread.h>
#include <stdlib.h>

// ldd: -lcrypto
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>

char* md5hash(char *str) {
  static __thread char hash[1 + 2*MD5_DIGEST_LENGTH];

  unsigned char md5[MD5_DIGEST_LENGTH] = {0};
  MD5((unsigned char*)str, strlen(str), md5);
  
  for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
    sprintf(hash + 2*i, "%02x", md5[i]);
  }
  return hash;
}

// vérifie si la chaîne 's' commence par 'n' caractères '0'
int zeros(char *s, int n) {
  for (int i = 0; i < n; i++) {
    if (s[i] != '0') {
      return 0;
    }
  }
  return 1;
}

// paramètres passés à chaque thread
struct thread_arg {
    int first;
    int step;
    int zero;
};

// tableau partagé des résultats
int nonces[5];
int count_nonces = 0;

// mutex pour protéger l'accès au tableau partagé
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// effectue un bruteforce pour trouver un entier dont le hachage MD5 commence par 'zero' caractères '0'
void *bruteforce(void *arg) {
  struct thread_arg *data = arg;
  char temp[7 + 8 * sizeof(int)];
  char* hash;
  int nbr = (*data).first;
  int trouve;
  
  while (1) {
    
    // arrêt si on a déjà trouvé assez de nonces
    pthread_mutex_lock(&lock);
    if (count_nonces >= 5) {
      pthread_mutex_unlock(&lock);
      break;
    }
    pthread_mutex_unlock(&lock);

    // test du nombre courant
    sprintf(temp, "%d", nbr);
    hash = md5hash(temp);
    trouve = zeros(hash, (*data).zero);
    // écriture dans le tableau partagé si hashage correct
    if (trouve == 1) {
      pthread_mutex_lock(&lock);
      if (count_nonces < 5) {
        nonces[count_nonces] = nbr;
        count_nonces++;
      }
      pthread_mutex_unlock(&lock); 
    }
    // on passe au nombre suivant
    nbr += (*data).step;
  }
  return NULL;
}

int main() {
  pthread_t threads[10];
  struct thread_arg data[10];

  // initialisation des paramètres de chaque thread
  for (int i = 0; i < 10; i++) {
    data[i].first = i;
    data[i].step = 10;
    data[i].zero = 3;
  }

  // création des threads
  for (int i = 0; i < 10; i++) {
    if (pthread_create(&threads[i], NULL, bruteforce, &data[i]) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  // attente de la fin des threads
  for (int i = 0; i < 10; i++) {
    pthread_join(threads[i], NULL);
  }
  
  // affichage des résultats
  printf("Nonces trouvés :\n");
  for (int i = 0; i < 5; i++) {
    char str_val[7 + 8 * sizeof(int)];
    sprintf(str_val, "%d", nonces[i]);
    printf("Nonce trouvé : %d ; Hash correspondant : %s\n", nonces[i], md5hash(str_val));
  }
}















