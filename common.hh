#pragma once

#include <vector>

struct Tile {
	char ch;
	int color;
	bool changed;

	bool operator==(const Tile& rhs) {
		return ch == rhs.ch && color == rhs.color;
	}
	bool operator!=(const Tile& rhs) { return !(*this==rhs); }
};

typedef std::vector< std::vector<Tile> > tilearray;
