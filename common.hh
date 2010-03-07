#pragma once

#include <cstdlib>
#include <vector>

#define BLOCKS true

struct Tile {
	char ch;
	int color;
	bool blocks_movement;
	bool blocks_vision;

	Tile(char ch = ' ', int color = 0, bool blocker = true):
	ch(ch), color(color), blocks_movement(blocker), blocks_vision(blocker) {}

	bool operator==(const Tile& rhs) {
		return ch == rhs.ch && color == rhs.color;
	}
	bool operator!=(const Tile& rhs) { return !(*this==rhs); }
};

typedef std::vector<Tile> tilerow;
typedef std::vector<tilerow> tilearray;

int inline randint(int hi) { return rand() % hi; }

int inline randint(int lo, int hi) {
	return (rand() % (hi - lo + 1)) + lo;
}
