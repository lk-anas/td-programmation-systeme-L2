// run:bash

// ldd: -lncurses
#include <ncurses.h>
#include <stdlib.h>

void INIT_SCREEN(void) {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
}

void DONE_SCREEN(void) {
  endwin();
}

#define PLAYERS 2
char CHIP[PLAYERS] = "XO";

// declaration des valeurs par défaut pour les dimensions 
int WIDTH = 16;
int HEIGHT = 16;

char **board;

void init_board(void) {
  int row, col;
  for (row = 0; row < HEIGHT; row++) {
    for (col = 0; col < WIDTH; col++) {
      board[row][col] = ' ';
    }
  }
}

void draw_board(void) {
  int row, col;
  move(0, 0);
  for (row = HEIGHT - 1; row >= 0; row--) {
    for (col = 0; col < WIDTH; col++) {
      printw("|%c", board[row][col]);
    }
    printw("|\n");
  }
  for (col = 0; col < WIDTH; col++) {
    printw("+-");
  }
  printw("+\n");
  for (col = 0; col < WIDTH; col++) {
    printw(" %c", 'A' + col);
  }
  printw("\n");
  refresh();
}

int get_col(void) {
  int key;
  while (1) {
    key = getch();
    if (key == KEY_BACKSPACE) {
      return -1;
    } else if ('A' <= key && key < 'A' + WIDTH) {
      return key - 'A';
    } else if ('a' <= key && key < 'a' + WIDTH) {
      return key - 'a';
    }
  }
}

int add_coin(int col, int player) {
  for (int row = 0; row < HEIGHT; row++) {
    if (board[row][col] == ' ') {
      board[row][col] = CHIP[player];
      return (player + 1) % PLAYERS;
    }
  }
  return player;
}

int game_over(void) {
  int full = 1;
  for (int row = 0; row < HEIGHT; row++) {
    for (int col = 0; col < WIDTH; col++) {
      if (board[row][col] == ' ') {
        full = 0;
        continue;
      }
      if (col < WIDTH - 3
          && board[row][col] == board[row][col + 1] &&
          board[row][col] == board[row][col + 2] &&
          board[row][col] == board[row][col + 3]) {
        return board[row][col];
      }
      if (row < HEIGHT - 3
          && board[row][col] == board[row + 1][col] &&
          board[row][col] == board[row + 2][col] &&
          board[row][col] == board[row + 3][col]) {
        return board[row][col];
      }
      if (row < HEIGHT - 3 && col < WIDTH - 3
          && board[row][col] == board[row + 1][col + 1] &&
          board[row][col] == board[row + 2][col + 2] &&
          board[row][col] == board[row + 3][col + 3]) {
        return board[row][col];
      }
      if (row < HEIGHT - 3 && col >= 3
          && board[row][col] == board[row + 1][col - 1] &&
          board[row][col] == board[row + 2][col - 2] &&
          board[row][col] == board[row + 3][col - 3]) {
        return board[row][col];
      }
    }
  }
  return full;
}

void play(void) {
  int turn = 0;
  int col;
  int winner;
  while (!(winner = game_over())) {
    draw_board();
    printw("\nplayer %c? ", CHIP[turn]);
    if ((col = get_col()) == -1) {
      winner = -1;
      break;
    }
    turn = add_coin(col, turn);
  }
  draw_board();
  if (winner <= 1) {
    printw("\n== no winner\n");
  } else {
    printw("\n== %c wins!!\n", winner);
  }
  printw("(hit a key)\n");
  refresh();
}

int main(int argc, char *argv[]) {
  
  unsigned int tmp;

  // si la ligne de commande ne fournis aucun arguments recevable 
  // on va recuperer les variables d'environnement (s'il existe) P4WIDTH et P4HEIGHT
  // si elles sont valides, on les utilise
  char *env1 = getenv("P4WIDTH");
  if (env1 != NULL) {
    tmp = atoi(env1);
    if ((tmp >= 4) && (tmp <= 26)) {
      WIDTH = tmp;
    }
  }

  char *env2 = getenv("P4HEIGHT");
  if (env2 != NULL) {
    tmp = atoi(env2);
    if ((tmp >= 4) && (tmp <= 16)) {
      HEIGHT = tmp;
    }
  }
  
  // vérifie si des arguments de ligne de commande ont été fournis
  // si oui on verifie leur valditié et on les donne (ou pas) aux variables WIDTH et HEIGTH
  switch (argc) {
    case 1:
      // pas d'aguments
      break;
    case 2:
      tmp = atoi(argv[1]);
      if (tmp >= 4 && tmp <= 26) {
        WIDTH = tmp;
      }
      break;
    case 3:
      tmp = atoi(argv[1]);
      if (tmp >= 4 && tmp <= 26) {
        WIDTH = tmp;
      }
      tmp = atoi(argv[2]);
      if (tmp >= 4 && tmp <= 16) {
        HEIGHT = tmp;
      }
      break;
    default:
      printf("erreur : on prend au maximum 2 arguments (width et height)\n");
      return 1;
  }

  // allocation dynamique du tableau
  board = malloc(HEIGHT * sizeof(char *));
  if (board == NULL) {
    printf("erreur d'allocation mémoire\n");
    return 1;
  }
  
  for (int i = 0; i < HEIGHT; i++) {
    board[i] = malloc(WIDTH * sizeof(char));
    if (board[i] == NULL){
      printf("erreur d'allocation mémoire\n");
      return 1;
    } 
  }
  
  INIT_SCREEN();
  init_board();
  play();
  getch();
  DONE_SCREEN();
  
  for (int i = 0; i < HEIGHT; i++) {
    free(board[i]);
  }
  free(board);
    
}



