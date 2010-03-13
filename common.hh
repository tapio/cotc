#pragma once

#include <curses.h>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <sstream>
#include <boost/circular_buffer.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "logger.hh"

#define KEY_ESCAPE 27
#define BLOCKS 1
#define BLOCKSMOVE 100

class Actor;
typedef std::vector<Actor*> ActorPtrs;
typedef boost::ptr_vector<Actor> Actors;

typedef boost::circular_buffer_space_optimized<std::string> MsgBuffer;

/// Tiles

struct Tile {
	const static int TRANSPARENT = 255;
	char ch;
	int color;
	std::string desc;
	bool explored;
	bool visible;
	bool blocks_movement;
	int blocks_vision_dist;
	Actor* actor;

	Tile(char ch = ' ', int color = 0, int blocker = BLOCKS):
	ch(ch), color(color), explored(false), visible(false),
	blocks_movement(blocker), blocks_vision_dist(blocker ? (blocker == BLOCKSMOVE ? TRANSPARENT : 0) : TRANSPARENT), actor(NULL) {}

	bool isFree() const { return (!blocks_movement) && (actor == NULL); }

	bool operator==(const Tile& rhs) {
		return ch == rhs.ch && color == rhs.color;
	}
	bool operator!=(const Tile& rhs) { return !(*this==rhs); }
};

typedef std::vector<Tile> tilerow;
typedef std::vector<tilerow> tilearray;

Tile TileBuilder(std::string type);


/// Math

template<typename T>
std::string num2str(T i) { std::ostringstream oss; oss << i; return oss.str(); }

bool inline randbool() { return rand() % 2 == 0; }

int inline randint(int hi) { return rand() % hi; }

int inline randint(int lo, int hi) {
	return (rand() % (hi - lo + 1)) + lo;
}

int inline randdir() { return randbool() ? 1 : -1; }
void inline swapdir(int& dir) { if (dir == 1) dir = -1; else dir = 1; }
void inline randdir(int& dx, int &dy) {
	if (randbool()) { dx = randdir(); dy = randint(-1,1); }
	else { dx = randint(-1,1); dy = randdir(); }
}

template<typename T> T inline distance2d(T x1, T y1, T x2, T y2) {
	return sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

template<typename T> T inline manhattan_dist(T x1, T y1, T x2, T y2) {
	return abs(x2-x1) + abs(y2-y1);
}

template<typename T> T inline max_axis_dist(T x1, T y1, T x2, T y2) {
	return std::max(abs(x2-x1),abs(y2-y1));
}

template<typename T> T inline min_axis_dist(T x1, T y1, T x2, T y2) {
	return std::min(abs(x2-x1),abs(y2-y1));
}

/** Implement C99 mathematical rounding (which C++ unfortunately currently lacks) **/
template <typename T> T round(T val) { return int(val + (val >= 0 ? 0.5 : -0.5)); }

/** Limit val to range [min, max] **/
template <typename T> T clamp(T val, T min = 0, T max = 1) {
	if (min > max) throw std::logic_error("min > max");
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

/// Console

void setColor(WINDOW* scr, int color);

void setColor(int color);

/// Function: addcstr
/// Draws a centered string.
void addcstr(std::string str);

/// Function: addrstr
/// Draws the string justified to the right.
void addrstr(std::string str);

void addrowstr(std::string str, size_t w = 0);

void addnum(int num);

void box2(int x, int y, int w, int h);

bool toggleDefaultColors();
