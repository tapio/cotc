#pragma once

#include "console.hh"
#include "common.hh"

class Actor;

class World: boost::noncopyable {
  public:
	World(): windowW(45), windowH(21) {
		scrX = COLS - 2 - windowW;
		scrY = LINES / 2 - windowH / 2 - 1;
		worldwin = newwin(windowH, windowW, scrY, scrX);
		viewXDist = windowW / 2 - 1;
		viewYDist = windowH / 2 - 1;
		generate();
	}

	~World() {
		actors.clear();
		delwin(worldwin);
	}

	/**
	 * Functions: Generators
	 */
	void generate(int seed = 0);
	void createCity(int xhouses = -1, int yhouses = -1);
	void createHouse(int x1, int y1, int x2, int y2, int furnit = 0, int locked = 0);
	void createPlaza(int x1, int y1, int x2, int y2);
	void randDoor(int x1, int y1, int x2, int y2, int locked = 0);
	void AddFurniture(int x1, int y1, int x2, int y2, int furnit, Tile floortype = Tile('.', COLOR_YELLOW, !BLOCKS), bool nobed = false);
	void makeDoor(int doorx, int doory, int locked = 0, Tile floortype = Tile('.', COLOR_YELLOW, !BLOCKS));
	void makeWallsAndFloor(int x1, int y1, int x2, int y2, Tile floortype = Tile('.', COLOR_YELLOW, !BLOCKS), bool nowindows = false, bool nowalls = false);

	Actor& addActor(Actor* actor);

	Tile getTile(int x, int y) const {
		if (x < 0 || y < 0 || x >= width || y >= height) return Tile();
		return tiles[y][x];
	}

	Tile* getTilePtr(int x, int y) {
		if (x < 0 || y < 0 || x >= width || y >= height) return NULL;
		return &tiles[y][x];
	}

	bool isFreeTile(int x, int y) const { return getTile(x,y).isFree(); }

	void updateView(Actor& actor);

	void updateVisibleActors();

	bool hasLOS(const Actor& actor, int x, int y) const;

	void update(bool skipAI = false);

	void draw(Actor& actor);

	int getWidth() const { return width; }
	int getHeight() const { return height; }

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
	Actors actors;
};

typedef World* WorldPtr;
