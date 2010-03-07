#include "world.hh"
#include "actor.hh"
#include "common.hh"
#include "logger.hh"

void setColor(WINDOW* scr, int color) {
	if (color < 0 || color > 15) return;
	if (color > 7) wattron(scr, A_BOLD);
	color = color % 8;
	wcolor_set(scr, color, 0);
}

void setColor(int color) {
	setColor(stdscr, color);
}

void World::generate(int w, int h) {
	width = w; height = h;
	for (int j = 0; j < h; j++) {
		tilerow row;
		for (int i = 0; i < w; i++) {
			Tile tile;
			if (i == 0 || i == w-1 || j == 0 || j == h-1 || randint(6) == 0) {
				tile = Tile('#', COLOR_CYAN, BLOCKS);
			} else tile = Tile('.', COLOR_GREEN, !BLOCKS);
			row.push_back(tile);
		}
		tiles.push_back(row);
	}
}

Actor& World::addActor(Actor* actor) {
	actors.push_back(actor);
	actors.back().setWorld(this);
	return actors.back();
}

/// Function: updateView
/// Updates the Actor's view.
void World::updateView(Actor& actor) {
	tilearray& view = actor.getView();
	for (int j = actor.y - viewYDist; j <= actor.y + viewYDist; j++) {
		for (int i = actor.x - viewXDist; i <= actor.x + viewXDist; i++) {
			if (i < 0 || j < 0 || i >= width || j >= height) continue;
			bool visible = hasLOS(actor, i, j);
			bool explored = visible ? true : actor.hasExplored(i,j);
			Tile& tile = view[j][i];
			if (visible || explored) tile = getTile(i, j);
			if (!visible) tile.actor = NULL;
			tile.visible = visible;
			tile.explored = explored;
		}
	}
}

/// Function updateVisibleActors
/// Updates all visibile actors lists of all actors
void World::updateVisibleActors() {
	for (Actors::iterator it = actors.begin(); it != actors.end(); it++) {
		it->visible_actors.clear();
	}
	for (Actors::iterator it = actors.begin(); it != actors.end(); it++) {
		for (Actors::iterator it2 = it+1; it2 != actors.end(); it2++) {
			if (it->getConstView()[it2->y][it2->x].visible) {
				it->visible_actors.push_back(&(*it2));
			}
		}
	}
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

/// Function: update
/// Updates the world - views, visibilities and AI
void World::update(bool skipAI) {
	// Reset tiles' actor pointers
	int w = getWidth(), h = getHeight();
	for (int j = 0; j < h; j++)
		for (int i = 0; i < w; i++) tiles[j][i].actor = NULL;
	// Update tiles' actor pointers
	for (Actors::iterator it = actors.begin(); it != actors.end(); ++it)
		tiles[it->y][it->x].actor = &(*it);
	// Update views
	for (Actors::iterator it = actors.begin(); it != actors.end(); ++it)
		updateView(*it);
	// Update visible actors
	updateVisibleActors();
	// Do AI
	if (skipAI) return;
	for (Actors::iterator it = actors.begin(); it != actors.end(); ++it)
		if (it->useAI) it->AI();
}

/// Function: draw
/// Draws the world with actors.
void World::draw(Actor& actor) {
	// Cls
	werase(worldwin);
	// Border
	wattroff(worldwin, A_BOLD);
	wcolor_set(worldwin, COLOR_GREEN, 0);
	box(worldwin, 0 , 0);
	// Tiles
	const tilearray& view = actor.getConstView();
	for (int j = actor.y - viewYDist; j <= actor.y + viewYDist; j++) {
		for (int i = actor.x - viewXDist; i <= actor.x + viewXDist; i++) {
			Tile tile;
			if (i >= 0 && j >= 0 && i < width && j < height) tile = view[j][i];
			if (tile.visible) wattron(worldwin, A_BOLD);
			else wattroff(worldwin, A_BOLD);
			wcolor_set(worldwin, tile.color, 0);
			mvwaddch(worldwin, y2scr(j, actor.y), x2scr(i, actor.x), tile.ch);
		}
	}
	// ActorPtrs
	for (ActorPtrs::const_iterator it = actor.visible_actors.begin(); it != actor.visible_actors.end(); ++it) {
		setColor(worldwin, (*it)->getColor());
		mvwaddch(worldwin, y2scr((*it)->y, actor.y),
						   x2scr((*it)->x, actor.x), (*it)->getChar());
	}
	// Player
	setColor(worldwin, actor.getColor());
	mvwaddch(worldwin, y2scr(actor.y, actor.y),
					   x2scr(actor.x, actor.x), actor.getChar());
	// Flush
	wrefresh(worldwin);
}
