#include "world.hh"
#include "actor.hh"
#include "common.hh"

void World::generate(int w, int h) {
	width = w; height = h;
	for (int j = 0; j < h; j++) {
		tilerow row;
		for (int i = 0; i < w; i++) {
			Tile tile;
			if (i == 0 || i == w-1 || j == 0 || j == h-1 || randint(10) == 0)
				tile = Tile('#', 1, BLOCKS);
			else tile = Tile('.', 2, !BLOCKS);
			row.push_back(tile);
		}
		tiles.push_back(row);
	}
}

tilearray World::getView(const Actor& actor) const {
	tilearray view;

	return view;
}

void World::draw(const Actor& actor) const {
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
