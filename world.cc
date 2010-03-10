#include "world.hh"
#include "actor.hh"
#include "common.hh"

namespace {
	// FOV algorithm from reflexivelos roguelike engine under MIT License.
	// Based on generalization of Bresenham, runs in O(N^3).
	void fov_dir(int dir, int dis, int px, int py, tilearray& view, World& world) {
		int cx, cy;
		for (int q = 1; q <= dis; ++q) for (int p = 0; p < q+1; p++) {
			for (int ad1 = 1, ad2[2] = {0, 0}, s[2] = {0, q-1}, eps[2] = {0, q-1}; ad1 <= dis && s[0] <= s[1]; ++ad1)
			  for (int i = 0; i < 2; i++) {
				eps[i] += p;
				if (eps[i] >= q) { // This should look familiar
					eps[i] -= q;
					++ad2[i];
				}
				cx = ad1, cy = ad2[i];
				if (dir&1) cx = -cx;
				if (dir&2) cy = -cy;
				if (dir&4) cx ^= cy ^= cx ^= cy;
				cx += px, cy += py;
				int d = (cx-px)*(cx-px) + (cy-py)*(cy-py); // Squared distance from origin
				// Visible
				if (!(cx < 0 || cy < 0 || cx >= world.getWidth() || cy >= world.getHeight())) {
					if (d <= dis*dis + 1) {
						view[cy][cx] = world.getTile(cx, cy);
						view[cy][cx].explored = true;
						view[cy][cx].visible = true;
					}
				}
				// Update range of possibilities for s
				int dd = world.getTile(cx, cy).blocks_vision_dist;
				if (d > dd*dd) { // if blocks_vision
					if (i == 0) s[i] += q-eps[i], eps[i] = 0, ++ad2[0];
					if (i == 1) s[i] -= eps[i]+1, eps[i] = q-1, --ad2[1];
				}
			}
		}
	}
}

void World::generate(int seed) {
	if (seed) srand(seed);
	createCity(10, 10);
}

Actor& World::addActor(Actor* actor) {
	actors.push_back(actor);
	actors.back().setWorld(this);
	return actors.back();
}

void World::removeDeadActors() {
	for (Actors::iterator it = actors.begin(); it != actors.end(); ) {
		if (it->isDead()) { it = actors.erase(it); }
		it++;
	}
}

/// Function: updateView
/// Updates the Actor's view.
void World::updateView(Actor& actor) {
	tilearray& view = actor.getView();
	// Reset light
	for (int j = actor.y - viewYDist; j <= actor.y + viewYDist; j++) {
		for (int i = actor.x - viewXDist; i <= actor.x + viewXDist; i++) {
			if (i < 0 || j < 0 || i >= width || j >= height) continue;
			view[j][i].visible = false;
		}
	}
	// Calculate FOV
	for (int i = 0; i < 8; ++i) fov_dir(i, actor.viewDist, actor.x, actor.y, view, *this);
}

/// Function updateVisibleActors
/// Updates all visibile actors lists of all actors
void World::updateVisibleActors() {
	for (Actors::iterator it = actors.begin(); it != actors.end(); it++) {
		it->visible_actors.clear();
	}
	for (Actors::iterator it = actors.begin(); it != actors.end(); it++) {
		for (Actors::iterator it2 = actors.begin(); it2 != actors.end(); it2++) {
			if (it == it2) continue;
			if (it->getConstView()[it2->y][it2->x].visible) {
				it->visible_actors.push_back(&(*it2));
			}
		}
	}
}

/// Function: hasLOS
/// Tests if given coordinates are visible for a player.
bool World::hasLOS(const Actor& actor, int x, int y) const {
	float dist = distance2d(float(x), float(y), float(actor.x), float(actor.y));

	if (dist > actor.viewDist) return false; // Out of reach
	if (x < 0 || y < 0 || x >= width || y >= height) return false; // Out of world
	if (dist <= 1.5) return true; // Very close

	float dx = float(x - actor.x) / (dist * 2.0);
	float dy = float(y - actor.y) / (dist * 2.0);
	float xx = actor.x + dx;
	float yy = actor.y + dy;
	float dd = distance2d(0.0f,0.0f,dx,dy);
	float d = 0.5 + dd;
	do {
		if (d > getTile(round(xx), round(yy)).blocks_vision_dist) return false;
		xx += dx; yy += dy; d += dd;
	} while (distance2d(float(x), float(y), xx, yy) > 0.5);
	return true;
}

/// Function: update
/// Updates the world - views, visibilities and AI
void World::update(bool skipAI) {
	// Clean up dead actors
	removeDeadActors();
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
	// Border
	wattroff(worldwin, A_BOLD);
	wcolor_set(worldwin, COLOR_GREEN, 0);
	box(worldwin, 0, 0);
	// Player
	setColor(worldwin, actor.getColor());
	mvwaddch(worldwin, y2scr(actor.y, actor.y),
					   x2scr(actor.x, actor.x), actor.getChar());

	// Flush
	wrefresh(worldwin);
}
