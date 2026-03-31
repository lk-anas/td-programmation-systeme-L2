#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

// fonction pour afficher les attributs d'un fichier sous la forme -rwxrwxrwx
void affiche_attribut(struct stat *st) {
  char mode[] = "----------";

  // détermine le type de fichier
  if (S_ISDIR((*st).st_mode)) {
    mode[0] = 'd';
  } else if (S_ISLNK((*st).st_mode)){
    mode[0] = 'l';
  } else if (S_ISFIFO((*st).st_mode)){
    mode[0] = 'p';
  } else if (S_ISSOCK((*st).st_mode)){
    mode[0] = 's';
  } else if (S_ISBLK((*st).st_mode)){
    mode[0] = 'b';
  } else if (S_ISCHR((*st).st_mode)){
    mode[0] = 'c';
  } else {
    mode[0] = '-';
  }
  
  // détermine les permissions
  if ((*st).st_mode & S_IRUSR){
    mode[1] = 'r';
  } 
  if ((*st).st_mode & S_IWUSR){
    mode[2] = 'w';
  } 
  if ((*st).st_mode & S_IXUSR){
   mode[3] = 'x';
  }
  
  if ((*st).st_mode & S_IRGRP){
    mode[4] = 'r';
  } 
  if ((*st).st_mode & S_IWGRP){
    mode[5] = 'w';
  }
  if ((*st).st_mode & S_IXGRP){
    mode[6] = 'x';
  } 
  
  if ((*st).st_mode & S_IROTH){
    mode[7] = 'r';
  } 
  if ((*st).st_mode & S_IWOTH){
    mode[8] = 'w';
  } 
  if ((*st).st_mode & S_IXOTH){
     mode[9] = 'x';
  }
  
  // bits spéciaux
  if (S_ISUID & (*st).st_mode){
    mode[3] = 's';
  }
  if (S_ISGID & (*st).st_mode){
    mode[6] = 's';
  } 
  if (S_ISVTX & (*st).st_mode){
    mode[9] = 't';
  } 

  printf("%s", mode);
}

// fonction pour afficher le nombre de lien d'un fichier
void affiche_nlink(struct stat *st) {
  printf(" %lu ", (*st).st_nlink);
}

// fonction pour afficher l'uid propietaire d'un fichier
void affiche_uid(struct stat *st) {
  printf("%u ", (*st).st_uid);
}

// fonction pour afficher le gid propietaire d'un fichier
void affiche_gid(struct stat *st) {
  printf("%u ", (*st).st_gid);
}

// fonction pour afficher la taille en octet d'un fichier
void affiche_size(struct stat *st) {
  printf("%lld ", (long long)(*st).st_size);
}

// fonction pour afficher le nom du fichier et si il y a un lien symbolique, afficher la cible
void affiche_nom_et_lien(struct dirent *entry, struct stat *st) {
  printf("%s", (*entry).d_name);
  
  if (S_ISLNK((*st).st_mode)) {
    char link_target[256];
    ssize_t length = readlink((*entry).d_name, link_target, sizeof(link_target)-1);
    if (length != -1) {
      link_target[length] = '\0';
      printf(" -> %s", link_target);
    }
  }
}

int main(void) {
  // ouvre le répertoire courant .
  // verifie si l'ouverture a réussi, sinon affiche erreur et quitte le programme
  DIR *dirp = opendir(".");
  if (dirp == NULL) {
    perror("opendir");
    exit(EXIT_FAILURE);
  }
  
  struct dirent *entry;
  struct stat statbuf;
  
  while (( entry = readdir(dirp)) != NULL) {
    if (lstat((*entry).d_name, &statbuf) == -1) {
      perror("lstat");
    }
    affiche_attribut(&statbuf);
	affiche_nlink(&statbuf);
	affiche_uid(&statbuf);
	affiche_gid(&statbuf);
	affiche_size(&statbuf);
	affiche_nom_et_lien(entry, &statbuf);
    printf("\n");
  }
  closedir(dirp);
}


