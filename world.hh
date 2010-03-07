#pragma once

#include <boost/noncopyable.hpp>
#include <vector>

#include "console.hh"
#include "common.hh"

class Actor;

class World: boost::noncopyable {
  public:
	World(): windowW(45), windowH(21) {
		scrX = COLS / 2 - windowW / 2;
		scrY = LINES / 2 - windowH / 2 - 1;
		worldwin = newwin(windowH, windowW, scrY, scrX);
		viewXDist = windowW / 2 - 1;
		viewYDist = windowH / 2 - 1;
		generate(100,100);
	}

	~World() {
		delwin(worldwin);
	}

	void generate(int w, int h);

	Tile getTile(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) return Tile();
		return tiles[y][x];
	}

	tilearray getView(const Actor& actor) const;

	bool hasLOS(const Actor& actor, int x, int y) const;

	void addActor(Actor* actor) { actors.push_back(actor); }

	void draw(const Actor& actor) const;

  private:
	int inline x2scr(int coord, int ref) const { return viewXDist + coord - ref + 1; }
	int inline y2scr(int coord, int ref) const { return viewYDist + coord - ref + 1; }
	int inline scr2x(int coord, int ref) const { return coord - viewXDist + ref - 1; }
	int inline scr2y(int coord, int ref) const { return coord - viewYDist + ref - 1; }

	WINDOW* worldwin;
	int windowW;
	int windowH;
	int scrX;
	int scrY;
	int width;
	int height;
	int viewXDist;
	int viewYDist;
	tilearray tiles;
	typedef std::vector<Actor*> Actors;
	Actors actors;

};

