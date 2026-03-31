#include <stdio.h>
#include <stdlib.h>

// 1) definition du type number
typedef struct {
  unsigned char base;
  unsigned int longueur;
  unsigned char* chiffres;
} number ;

// 2) fonction pour liberer la mémoire occupé par une variable de type number 
void free_number(number nbr){
  if(nbr.chiffres != NULL){
    free(nbr.chiffres);
  }
}

// 3) fonction qui convertit un entier C en une base choisi
unsigned int find_length(unsigned int nbr,unsigned char base){
  unsigned int length = 0;
  
  if(nbr == 0){
    return 1;
  }
  
  while (nbr > 0){
    nbr = nbr / base ;
    length++;
  }
  
  return length;
}

number to_number(unsigned int nbr, unsigned char base){
  if((base != 2) && (base != 4) && (base != 8) && (base != 16) && (base != 32) && (base != 64)){
    base = 16;
  }
  
  number res;
  
  res.base = base;
  
  res.longueur = find_length(nbr,base);
  
  res.chiffres = malloc(res.longueur*sizeof(unsigned char));
  for(int i = res.longueur-1 ; i >= 0 ; i--){
    res.chiffres[i] = nbr % base;
    nbr = nbr/base;
  }
  
  return res;
}

// 4) fonction qui convertit un nombre en une base choisi en entier C
unsigned int to_uint(number nbr){
  unsigned int res = 0;
  for(unsigned int i=0 ; i< nbr.longueur ; i++){
    res = res*nbr.base + nbr.chiffres[i];
  }
  return res;
}

// 5) fonction qui renvoie la chaine de caractères représentant le nombre dans sa base
char chiffres[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

char* to_string(number nbr){
  char* res = malloc((nbr.longueur + 1) * sizeof(char));
  for(unsigned int i = 0; i < nbr.longueur; i++){
    res[i] = chiffres[nbr.chiffres[i]];
  }
  res[nbr.longueur] = '\0';
  return res;
}


int main(void) {
}


