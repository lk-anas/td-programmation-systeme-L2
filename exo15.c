// ldd: -lcrypto
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

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

// Vérifie si la chaîne 's' commence par 'n' caractères '0'
int zeros (char *s, int n){
  for(int i=0; i<n; i++){
    if(s[i] != '0'){
      return 0;
    }
  }
  return 1;
} 

// Effectue un bruteforce pour trouver un entier dont le hachage MD5 commence par 'zero' caractères '0'
void bruteforce (int first, int step, int zero){

  char nbr[128];
  char nom_PID[128];
 
  for (int i=first; ; i+=step){

    sprintf(nbr, "%d", i);
    char *hashage = md5hash(nbr);

    if (zeros(hashage, zero) == 1){

      sprintf(nom_PID, "found.%d", getpid());

      int fichier_ouvert = open(nom_PID, O_WRONLY | O_CREAT | O_TRUNC, 0600);
      if (fichier_ouvert == -1){
        perror("ouverture du fichier");
        exit(EXIT_FAILURE);
      }

      int fichier_ecrit = write(fichier_ouvert, &i, sizeof(int));
      if (fichier_ecrit < sizeof(int)){
        perror("ecriture du fichier");
        exit(EXIT_FAILURE);
      }

      int fichier_ferme = close(fichier_ouvert);
      if (fichier_ferme == -1){
        perror("fermeture du fichier");
        exit(EXIT_FAILURE);
      }

      exit(EXIT_SUCCESS);
    }
  }
}

int main (void){

  // Lancement de 10 processus enfants pour répartir le travail de bruteforce
  pid_t PGID = -1;
  for(int i=0; i<10; i++){
    pid_t p = fork();
    if(p < 0){
      perror("création du processus enfant");
      exit(EXIT_FAILURE);
    } else if (p == 0){
      bruteforce(i, 10, 6);
    } else {    
      if(PGID == -1) {
        PGID = p;
      }   
      setpgid(p,PGID);     
    }
  }
  
  // Attendre le premier enfant qui termine avec le code de retour 0
  int temp = 1;
  int wstatus;
  pid_t pid_win;

  while (temp == 1) {
    pid_win = wait(&wstatus);
    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0) {
      temp = 0;
    }
  }
 
  // Tuer tous les autres processus enfants
  kill(-PGID, SIGKILL); 
  
  // lit le fichier found.PID correspondant à l’enfant qui a été attendu pour afficher la valeur de l’entier enregistré dedans 
  char result[128] ;
  int winner ;
  
  sprintf(result, "found.%i", pid_win);
  
  int f = open(result, O_RDONLY);
  if (f == -1) {
    perror("ouverture");
    exit(EXIT_FAILURE);
  }

  int res = read(f, &winner, sizeof(int));
  if (res < sizeof(int)) {
    perror("lecture");
    exit(EXIT_FAILURE);
  }
  
  int fermeture = close(f);
  if (fermeture == -1) {
    perror("fermeture");
    exit(EXIT_FAILURE);
  }
  
  int suppr = unlink(result);
  if (suppr == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
 
  // Afficher la valeur de l'entier trouvé (avec le hashage correspondant) 
  char winner_str[128];
  sprintf(winner_str, "%d", winner);
  
  char *hash_winner = md5hash(winner_str);
 
  printf("bitcoin trouvé : %d\n", winner);
  printf("hashage correspondant : %s\n", hash_winner);

}
























