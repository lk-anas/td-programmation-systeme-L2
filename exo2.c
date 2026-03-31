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
  exit(0);
}

#define PLAYERS 2
char CHIP[PLAYERS] = "XO";

#define SIZE 16

char board[SIZE][SIZE];

void init_board(void) {
  int row, col;
  for (row = 0; row < SIZE; row++) {
    for (col = 0; col < SIZE; col++) {
      board[row][col] = ' ';
    }
  }
}

void draw_board(void) {
  int row, col;
  move(0, 0);
  for (row = SIZE - 1; row >= 0; row--) {
    for (col = 0; col < SIZE; col++) {
      printw("|%c", board[row][col]);
    }
    printw("|\n");
  }
  for (col = 0; col < SIZE; col++) {
    printw("+-");
  }
  printw("+\n");
  for (col = 0; col < SIZE; col++) {
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
    } else if ('A' <= key && key < 'A' + SIZE) {
      return key - 'A';
    } else if ('a' <= key && key < 'a' + SIZE) {
      return key - 'a';
    }
  }
}

int add_coin(int col, int player) {
  for (int row = 0; row < SIZE; row++) {
    if (board[row][col] == ' ') {
      board[row][col] = CHIP[player];
      return (player + 1) % PLAYERS;
    }
  }
  return player;
}

int game_over(void) {
  int full = 1;
  for (int row = 0; row < SIZE; row++) {
    for (int col = 0; col < SIZE; col++) {
      if (board[row][col] == ' ') {
        full = 0;
        continue;
      }
      if (col < SIZE - 3 
          && board[row][col] == board[row][col + 1] &&
          board[row][col] == board[row][col + 2] &&
          board[row][col] == board[row][col + 3]) {
        return board[row][col];
      }
      if (row < SIZE - 3 
          && board[row][col] == board[row + 1][col] &&
          board[row][col] == board[row + 2][col] &&
          board[row][col] == board[row + 3][col]) {
        return board[row][col];
      }
      if (row < SIZE - 3 && col < SIZE - 3
          && board[row][col] == board[row + 1][col + 1] &&
          board[row][col] == board[row + 2][col + 2] &&
          board[row][col] == board[row + 3][col + 3]) {
        return board[row][col];
      }
      if (row < SIZE - 3 && col >= 3
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
    printw("\n== %c wins!\n", winner);
  }
  printw("appuyez sur une touche \n");
  refresh();
}

int main(void) {
  INIT_SCREEN();
  init_board();
  play();
  getch();
  DONE_SCREEN();
}

