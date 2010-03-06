#include "rlutil.h"
#include <cstdlib> // for srand() / rand()

using namespace rlutil;

/// Tiles
#define FLOOR 0
#define WALL 1
#define COIN (1 << 1)
#define STAIRS_DOWN (1 << 2)
#define TORCH (1 << 4)

#define MAPSIZE 15

/// Globals
int x, y;
int coins = 0, moves = 0, torch = 30, level = 1;
int lvl[MAPSIZE][MAPSIZE];

/// Generates the dungeon map
void gen(int seed) {
	srand(seed);
	for (int j = 0; j < MAPSIZE; j++) {
		for (int i = 0; i < MAPSIZE; i++) {
			if (i == 0 || i == MAPSIZE-1 || j == 0 || j == MAPSIZE-1 ||
			  rand() % 10 == 0) lvl[i][j] = 1;
			else if (rand() % 20 == 0) lvl[i][j] = COIN;
			else if (rand() % 100 == 0) lvl[i][j] = TORCH;
			else lvl[i][j] = 0;
		}
	}
	#define randcoord (1+rand()%(MAPSIZE-2))
	x = randcoord;
	y = randcoord;
	lvl[randcoord][randcoord] = STAIRS_DOWN;
}

/// Draws the screen
void draw() {
	cls();
	std::cout << ANSI_YELLOW << "Coins: " << ANSI_WHITE << coins << std::endl;
	std::cout << ANSI_RED << "Torch: " << ANSI_WHITE << torch << std::endl;
	std::cout << ANSI_MAGENTA << "Moves: " << ANSI_WHITE << moves << std::endl;
	std::cout << ANSI_GREEN << "Level: " << ANSI_WHITE << level << std::endl;
	for (int j = 0; j < MAPSIZE; j++) {
		for (int i = 0; i < MAPSIZE; i++) {
			if (i == x && j == y) std::cout << ANSI_WHITE << "@";
			else if (abs(x-i)+abs(y-j)>std::min(10,torch/2)) std::cout << " ";
			else if (lvl[i][j] == 0) std::cout << ANSI_BLUE << ".";
			else if (lvl[i][j] & WALL) std::cout << ANSI_CYAN << "#";
			else if (lvl[i][j] & COIN) std::cout << ANSI_YELLOW << "o";
			else if (lvl[i][j] & STAIRS_DOWN) std::cout << ANSI_GREEN << "<";
			else if (lvl[i][j] & TORCH) std::cout << ANSI_BROWN << "f";
		}
		std::cout << std::endl;
	}
}

/// Main loop and input handling
int main() {
	gen(level);
	std::cout << ANSI_GREEN << "Welcome! Use WASD to move." << std::endl;
	std::cout << ANSI_BROWN << "Hit any key to start." << std::endl;
	anykey();
	draw();
	while (true) {
		// Input
		if (kbhit()) {
			char k = getch();

			std::cout << "#" << k << "#" << std::endl;

			int oldx = x, oldy = y;
			if (k == 'a') { --x; ++moves; }
			else if (k == 'd') { ++x; ++moves; }
			else if (k == 'w') { --y; ++moves; }
			else if (k == 's') { ++y; ++moves; }
			else if (k == 27) exit(0);
			// Collisions
			if (lvl[x][y] & WALL) { x = oldx; y = oldy; }
			else if (lvl[x][y] & COIN) { coins++; lvl[x][y] ^= COIN; }
			else if (lvl[x][y] & TORCH) { torch+=20; lvl[x][y] ^= TORCH; }
			else if (lvl[x][y] & STAIRS_DOWN) gen(++level);
			// Drawing
			draw();
			// Die
			if (--torch <= 0) break;
		}
		//msleep(200);
	}

	return 0;
}


