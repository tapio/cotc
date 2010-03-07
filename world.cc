#include "world.hh"
#include "actor.hh"
#include "common.hh"
#include "logger.hh"

void World::generate(int w, int h) {
	width = w; height = h;
	for (int j = 0; j < h; j++) {
		tilerow row;
		for (int i = 0; i < w; i++) {
			Tile tile;
			if (i == 0 || i == w-1 || j == 0 || j == h-1 || randint(6) == 0)
				tile = Tile('#', 1, BLOCKS);
			else tile = Tile('.', 2, !BLOCKS);
			row.push_back(tile);
		}
		tiles.push_back(row);
	}
}

/// Function: getView
/// Creates an array of <Tiles> from the actors view.
tilearray World::getView(const Actor& actor) const {
	tilearray view;
	for (int j = 1; j < viewYDist*2 + 2; j++) {
		tilerow row;
		for (int i = 1; i < viewXDist*2 + 2; i++) {
			Tile tile;
			int x = scr2x(i, actor.x);
			int y = scr2y(j, actor.y);
			if (hasLOS(actor, x, y)) tile = getTile(x, y);
			row.push_back(tile);
		}
		view.push_back(row);
	}
	return view;
}

/// Function: hasLOS
/// Tests if given coordinates are visible for a player.
bool World::hasLOS(const Actor& actor, int x, int y) const {
	int dist = distance2d(x, y, actor.x, actor.y);

	if (dist > actor.viewDist) return false; // Out of reach
	if (x < 0 || y < 0 || x >= width || y >= height) return false; // Out of world
	if (dist <= 1.5) return true; // Very close

	float dx = float(x - actor.x) / (dist * 2.0);
	float dy = float(y - actor.y) / (dist * 2.0);
	float xx = actor.x + dx;
	float yy = actor.y + dy;
	do {
		if (getTile(xx, yy).blocks_vision) return false;
		xx += dx; yy += dy;
		if (distance2d<float>(x,y,xx,yy) < 0.5) return true;
	} while (int(xx) != x || int(yy) != y);
	return true;
}

/// Function: draw
/// Draws the world with actors.
void World::draw(const Actor& actor) const {
	// Cls
	werase(worldwin);
	// Border
	wcolor_set(worldwin, COLOR_GREEN, 0);
	box(worldwin, 0 , 0);
	// Tiles
	tilearray view = getView(actor);
	for (int j = 0; j < viewYDist*2 + 1; j++) {
		for (int i = 0; i < viewXDist*2 + 1; i++) {
			Tile tile = view[j][i];
			wcolor_set(worldwin, tile.color, 0);
			mvwaddch(worldwin, j+1, i+1, tile.ch);
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
