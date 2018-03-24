#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gameboard.h"

#define win_height board_height+2
#define win_width board_width+2

#define VERSION 1

const char numbers[] = {'.','1','2','3','4','5','6','7','8'};

void drawBoard(WINDOW *w, game_board *g);
void drawScore(WINDOW *w, game_board *g);
int command(game_board *g, WINDOW *w, int cur[2]);
int placeFlag(game_board *g, int y, int x);
int dig(game_board *g, int y, int x);
int recursiveDig(game_board *g, int y, int x);

int main(int argc, char *argv[])
{
	int cursor[2] = {board_height/2,board_width/2};
	srand(time(NULL));
	struct results res;
	game_board *game = new_game(130,15); // bomb total, chance

	initscr();
	cbreak();noecho();
	WINDOW *gamewin = newwin(win_height, win_width, 0, 0);
	box(gamewin,0,0);wrefresh(gamewin);keypad(gamewin,TRUE);
	WINDOW *scorewin = newwin(win_height, 76-win_width, 0, win_width);
	box(scorewin,0,0);wrefresh(scorewin);


	do {
		drawBoard(gamewin,game);
		drawScore(scorewin,game);
		wmove(gamewin,cursor[0],cursor[1]);
	} while (command(game,gamewin,cursor));

	delwin(gamewin);
	delwin(scorewin);
	endwin();
	game->score = game->correct_flags*30;
	cleanup_game(game, &res);

	fprintf(stdout,"FINAL RESULTS:\n\
	Total Bombs: %i\n\
	Total Flags: %i\n\
	Correct Flags: %i\n\
	Final Score: %i\n",res.bomb_total, res.flag_total, res.correct_flags, res.score);

	return 0;
}

void drawBoard(WINDOW *w, game_board *g){
	int x,y;
	int c_target = 0;
	char target = '!';
	for (y=0;y<board_height;y++) {
		for (x=0;x<board_width;x++) {
			target = numbers[getNeighbors(g->board[y][x])];
			c_target = getNeighbors(g->board[y][x]);

			if (getHidden(g->board[y][x])) target = ' ';
			if (getFlagged(g->board[y][x])) target = '#';

			c_target = getNeighbors(g->board[y][x]);
			if (c_target == 0 || target == '#') c_target = 9;
			mvwaddch(w,1+y,1+x,target);
		}
	}
	wrefresh(w);
}

void drawScore(WINDOW *w, game_board *g){
	mvwprintw(w,1,((76-win_width)/2)-7,"MineSweeper v%i",VERSION); // 14
	mvwprintw(w,4,1,"Score: %i",g->score);
	mvwprintw(w,5,1,"Bombs: %i",g->bomb_total);
	mvwprintw(w,6,1,"Flagged: %i",g->flag_total);

	wrefresh(w);
}

int command(game_board *g, WINDOW *w, int cur[2]){
	char c = wgetch(w);
	switch(c) {
		case 'h':
			if (cur[1]-1 > 0) cur[1] -= 1;
			break;
		case 'j':
			if (cur[0]+1 <= board_height) cur[0] += 1;
			break;
		case 'k':
			if (cur[0]-1 > 0) cur[0] -= 1;
			break;
		case 'l':
			if (cur[1]+1 <= board_width) cur[1] += 1;
			break;

		case 'm':
		case 'f':
			return placeFlag(g,cur[0]-1,cur[1]-1);
			break;

		case 'n':
		case 'd':
			if (!getFlagged(g->board[cur[0]-1][cur[1]-1]))
				if (dig(g,cur[0]-1,cur[1]-1) != 0) return 0;
			break;

		case 'q':
			return 0;
			break;
		default:
			return 1;
			break;
	}
	return 1;
}

int placeFlag(game_board *g, int y, int x) {
	if (!getHidden(g->board[y][x])) {
		int rv=1,neighbor_total=0;
		int up=1,down=1,left=1,right=1;

		if (y == 0) up = 0;
		if (y == board_height - 1) down = 0;
		if (x == 0) left = 0;
		if (x == board_width - 1) right = 0;

		if (up) {
			if (getFlagged(g->board[y-1][x])) neighbor_total += 1;
			if (left && getFlagged(g->board[y-1][x-1])) neighbor_total += 1;
			if (right && getFlagged(g->board[y-1][x+1])) neighbor_total += 1;
		}
		if (down) {
			if (getFlagged(g->board[y+1][x])) neighbor_total += 1;
			if (left && getFlagged(g->board[y+1][x-1])) neighbor_total += 1;
			if (right && getFlagged(g->board[y+1][x+1])) neighbor_total += 1;
		}
		if (left && getFlagged(g->board[y][x-1])) neighbor_total += 1;
		if (right && getFlagged(g->board[y][x+1])) neighbor_total += 1;

		if (neighbor_total == getNeighbors(g->board[y][x])) {
			g->board[y][x] = setHidden(g->board[y][x]);
			if (!recursiveDig(g,y,x)) return 0;
		}
		return rv;
	}
	if (!getFlagged(g->board[y][x])) {
		g->board[y][x] = setFlagged(g->board[y][x]);
		g->flag_total++;
		g->score += 30;
		if (getBomb(g->board[y][x])) g->correct_flags++;
	} else {
		g->board[y][x] = setFlagged(g->board[y][x]);
		g->flag_total--;
		g->score -= 30;
		if (getBomb(g->board[y][x])) g->correct_flags--;
	}
	if (g->correct_flags == g->bomb_total) {
			g->score += (g->correct_flags*30) + 100*g->bomb_total;
			return 0;
	}
	return 1;
}

int dig(game_board *g, int y, int x) {
	int rv = 0;
	if (getBomb(g->board[y][x])) return 1;
	if (getNeighbors(g->board[y][x]) == 0) return recursiveDig(g,y,x);

	if (getHidden(g->board[y][x]) && !getFlagged(g->board[y][x])) g->board[y][x] = setHidden(g->board[y][x]);
	return rv;
}

int recursiveDig(game_board *g, int y, int x) {
	int rv = 0;
	int right=1,left=1,up=1,down=1;

	if (getBomb(g->board[y][x])) return 1;
	if (getHidden(g->board[y][x])) {
		g->board[y][x] = setHidden(g->board[y][x]);

		if (y == 0) up = 0;
		if (y == board_height - 1) down = 0;
		if (x == 0) left = 0;
		if (x == board_width - 1) right = 0;

		if (up) {
			rv += dig(g,y-1,x);
			if (left) rv += dig(g,y-1,x-1);
			if (right) rv += dig(g,y-1,x+1);
		}

		if (down) {
			rv += dig(g,y+1,x);
			if (left) rv += dig(g,y+1,x-1);
			if (right) rv += dig(g,y+1,x+1);
		}

		if (left) rv += dig(g,y,x-1);
		if (right) rv += dig(g,y,x+1);
	}
	return rv;
}
