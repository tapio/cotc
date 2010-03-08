#pragma once

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <curses.h>
#include <boost/ptr_container/ptr_vector.hpp>

#define KEY_ESCAPE 27
#define BLOCKS true

class Actor;
typedef std::vector<Actor*> ActorPtrs;
typedef boost::ptr_vector<Actor> Actors;

struct Tile {
	char ch;
	int color;
	bool explored;
	bool visible;
	bool blocks_movement;
	bool blocks_vision;
	Actor* actor;

	Tile(char ch = ' ', int color = 0, bool blocker = true):
	ch(ch), color(color), explored(false), visible(false),
	blocks_movement(blocker), blocks_vision(blocker), actor(NULL) {}

	bool isFree() const { return (!blocks_movement) && (actor == NULL); }

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

