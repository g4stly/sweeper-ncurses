#include <stdlib.h>
#include <time.h>
#include "gameboard.h"
#include "random.h"

#define BOMB_FLAG 8
#define HIDDEN_FLAG 4
#define FLAGGED_FLAG 2

game_board *new_game(int b_total, int b_chance)
{
  int i,j,target;
  int x,y;
  int look_down,look_left,look_right;
  game_board *local_game = NULL;
  local_game = malloc(sizeof(game_board));
  local_game->score = 0;
  local_game->bomb_total = 0;
  local_game->flag_total = 0;
  local_game->correct_flags = 0;


  for (i=0;i<board_height;i++) { // create all empty tiles
    for (j=0;j<board_width;j++) {
      target = 0;
      local_game->board[i][j] = craftTile(0,target,1,0);
    }
  }

  do { // fill with bombs
    x = randomInt(board_width-1);
    y = randomInt(board_height-1);
    if (!getBomb(local_game->board[y][x])) {
      local_game->board[y][x] = setBomb(local_game->board[y][x]);
      local_game->bomb_total += 1;
    }
  } while (local_game->bomb_total < b_total);

  for (i=0;i<board_height;i++) { // correct neighbor values
    if (i==board_height-1) {look_down = 0;} else {look_down = 1;} // last row
    for (j=0;j<board_width;j++) {
      if (j==0) {look_left = 0;} else {look_left = 1;} // left col
      if (j==board_width-1) {look_right = 0;} else {look_right = 1;} //right col

      target = 0;
      if (i) {if (getBomb(local_game->board[i-1][j])) target += 1;}
      if (look_down) {if (getBomb(local_game->board[i+1][j])) target += 1;}
      if (look_left) {if (getBomb(local_game->board[i][j-1])) target += 1;}
      if (look_right) {if (getBomb(local_game->board[i][j+1])) target += 1;}

      if (i) {
        if (look_left) {if (getBomb(local_game->board[i-1][j-1])) target += 1;}
        if (look_right) {if (getBomb(local_game->board[i-1][j+1])) target += 1;}
      }
      if (look_down) {
        if (look_left) {if (getBomb(local_game->board[i+1][j-1])) target += 1;}
        if (look_right) {if (getBomb(local_game->board[i+1][j+1])) target += 1;}
      }
      local_game->board[i][j] = setNeighbors(local_game->board[i][j],target);
    }
  }

  return local_game;
}

void cleanup_game(game_board *g, struct results *r)
{
  r->score = g->score;
  r->bomb_total = g->bomb_total;
  r->flag_total = g->flag_total;
  r->correct_flags = g->correct_flags;
  free(g);
}

// tile functions

tile craftTile(int neighbors, int bomb, int hidden, int flagged) {
  tile t = 0;
  setNeighbors(t, neighbors);
  if (bomb) t = setBomb(t);
  if (hidden) t = setHidden(t);
  if (flagged) t = setFlagged(t);
  return t;
}

tile setNeighbors(tile t, int count) {
  if (count < 9) {
    tile c = (tile)count; // read count into a byte
    c = c << 4; // shift the 4 bit number to left 4 bits
    t = t & (uint8_t)15; // 15=00001111; AND it to maintain flags and set neighbors to 0
    return (t | c); // c=xxxx0000; OR it to maintain flags and apply neighbors
  } else return t; // count too big, must be an error
}

tile getNeighbors(tile t) {return (t >> 4);} // shift the flags out of the byte

tile setBomb(tile t) {return (t ^ BOMB_FLAG);}
tile getBomb(tile t) {return (t & BOMB_FLAG);}

tile setHidden(tile t) {return (t ^ HIDDEN_FLAG);}
tile getHidden(tile t) {return (t & HIDDEN_FLAG);}

tile setFlagged(tile t) {return (t ^ FLAGGED_FLAG);}
tile getFlagged(tile t) {return (t & FLAGGED_FLAG);}

