#ifndef game_board_h
#define game_board_h

#include <stdint.h>

#define board_width 46
#define board_height 22

typedef uint8_t tile;

/*
tile byte breakdown:

first four bits are neighbor bits
then comes the bomb bit, the hidden bit,
then the flag bit, then a junk bit.

*/
struct results {
  int bomb_total;
  int flag_total;
  int correct_flags;
  int score;
};

typedef struct {
  int bomb_total;
  int flag_total;
  int correct_flags;
  int score;
  tile board[board_height][board_width];
} game_board;

game_board *new_game(int b_total, int b_chance);
void cleanup_game(game_board *g, struct results *r);
// tile functions

tile craftTile(int neighbors, int bomb, int hidden, int flagged);

tile setNeighbors(tile t, int count);
tile getNeighbors(tile t);

tile setBomb(tile t);
tile getBomb(tile t);

tile setHidden(tile t);
tile getHidden(tile t);

tile setFlagged(tile t);
tile getFlagged(tile t);

tile setDrawBit(tile t);
tile getDrawBit(tile t);



#endif
