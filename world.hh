#pragma once

#include <boost/noncopyable.hpp>
#include <vector>

#include "console.hh"
#include "actor.hh"
#include "common.hh"

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

	void generate(int w, int h) {
		width = w; height = h;
		for (int j = 0; j < h; j++) {
			tilerow row;
			for (int i = 0; i < w; i++) {
				Tile tile('.', 1);
				if (i == 0 || i == w-1 || j == 0 || j == h-1 || randint(10) == 0)
					tile.ch = '#';
				row.push_back(tile);
			}
			tiles.push_back(row);
		}
	}

	Tile getTile(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) return Tile();
		return tiles[y][x];
	}

	void addActor(Actor& actor) { actors.push_back(&actor); }

	void draw(const Actor& actor) const {
		// Cls
		werase(worldwin);
		// Border
		wcolor_set(worldwin, COLOR_GREEN, 0);
		box(worldwin, 0 , 0);
		// Tiles
		for (int j = actor.y - viewYDist; j <= actor.y + viewYDist; j++) {
			for (int i = actor.x - viewXDist; i <= actor.x + viewXDist; i++) {
				Tile tile = getTile(i,j);
				wcolor_set(worldwin, tile.color, 0);
				mvwaddch(worldwin, y2scr(j, actor.y), x2scr(i, actor.x), tile.ch);
			}
		}
		// Actors
		for (Actors::const_iterator it = actors.begin(); it != actors.end(); it++) {
			wcolor_set(worldwin, (*it)->getColor(), 0);
			mvwaddch(worldwin, y2scr((*it)->y, actor.y),
			                   x2scr((*it)->x, actor.x), (*it)->getChar());
		}
		// Flush
		wrefresh(worldwin);
	}

  private:
	int inline x2scr(int coord, int ref) const { return viewXDist + coord - ref + 1; }
	int inline y2scr(int coord, int ref) const { return viewYDist + coord - ref + 1; }

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

