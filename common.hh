#pragma once

#include <cstdlib>
#include <vector>
#include <cmath>
#include <curses.h>

#define BLOCKS true

struct Tile {
	char ch;
	int color;
	bool explored;
	bool visible;
	bool blocks_movement;
	bool blocks_vision;

	Tile(char ch = ' ', int color = 0, bool blocker = true):
	ch(ch), color(color), explored(false), visible(false),
	blocks_movement(blocker), blocks_vision(blocker) {}

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

template<typename T> T distance2d(T x1, T y1, T x2, T y2) {
	return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

