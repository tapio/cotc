#pragma once

#include <cstdlib>
#include <vector>

struct Tile {
	char ch;
	int color;
	bool changed;

	Tile(char ch = ' ', int color = 0): ch(ch), color(color) {}

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
