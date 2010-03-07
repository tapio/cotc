#pragma once

#include "world.hh"

#define NO_AI false

class World;

class Actor {
  public:
	enum Type { HUMAN, ANGEL, ARCHANGEL, IMP, DEMON, ARCHDEMON } type;

	Actor(Type type, bool ai = true): type(type), x(), y(),
	  viewDist(10), useAI(ai), world() { }

	void setWorld(World* wd) {
		world.reset(wd);
		// Construct view array
		int w = world->getWidth();
		int h = world->getHeight();
		for (int j = 0; j < h; j++) {
			tilerow row;
			for (int i = 0; i < w; i++) row.push_back(Tile());
			view.push_back(row);
		}
	}

	void AI() {
		switch (type) {
			case HUMAN: AI_human(); break;
			case ANGEL: case ARCHANGEL: AI_angel(); break;
			case IMP: case DEMON: case ARCHDEMON: AI_demon(); break;
		}
	}

	void move(int dx, int dy) {
		if (world->isFreeTile(x+dx, y+dy)) {
			x+=dx;
			y+=dy;
		}
	}

	bool position(int newx, int newy) {
		if (world->isFreeTile(newx, newy)) {
			x = newx; y = newy;
			return true;
		}
		return false;
	}

	char getChar() const {
		switch(type) {
			case HUMAN:     return 'h';
			case ANGEL:     return 'a';
			case ARCHANGEL: return 'A';
			case IMP:       return 'i';
			case DEMON:     return 'd';
			case ARCHDEMON: return 'D';
		}
		return '\0';
	}

	int getColor() const {
		switch(type) {
			case HUMAN:     return COLOR_YELLOW;
			case ANGEL:     return COLOR_WHITE;
			case ARCHANGEL: return COLOR_WHITE;
			case IMP:       return COLOR_RED;
			case DEMON:     return COLOR_RED;
			case ARCHDEMON: return COLOR_RED;
		}
		return -1;
	}

	tilearray& getView() { return view; }

	const tilearray&  getConstView() const { return view; }

	bool hasExplored(int x, int y) const {
		if (x < 0 || y < 0 || x >= world->getWidth() || y >= world->getHeight()) return false;
		return view[y][x].explored;
	}

	int x;
	int y;
	int viewDist;
	bool useAI;

	ActorPtrs visible_actors;

  private:
	WorldPtr world;
	tilearray view;

	void AI_angel();
	void AI_human();
	void AI_demon();
};
