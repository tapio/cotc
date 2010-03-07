#pragma once

#include "world.hh"

class World;

class Actor {
  public:
	enum Type { HUMAN, ANGEL, ARCHANGEL, IMP, DEMON, ARCHDEMON } type;

	Actor(World& wd, Type type): type(type), x(), y(), viewDist(10), world(wd) {
		world.addActor(this);
		// Construct view array
		int w = world.getWidth();
		int h = world.getHeight();
		for (int j = 0; j < h; j++) {
			tilerow row;
			for (int i = 0; i < w; i++) row.push_back(Tile());
			view.push_back(row);
		}
	}

	void move(int dx, int dy) {
		if (!world.getTile(x+dx, y+dy).blocks_movement) {
			x+=dx;
			y+=dy;
		}
	}

	void position(int newx, int newy) {
		x = newx;
		y = newy;
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
			case HUMAN:     return 1;
			case ANGEL:     return 3;
			case ARCHANGEL: return 3;
			case IMP:       return 2;
			case DEMON:     return 2;
			case ARCHDEMON: return 2;
		}
		return -1;
	}

	tilearray& getView() { return view; }

	const tilearray&  getConstView() const { return view; }

	bool hasExplored(int x, int y) const {
		if (x < 0 || y < 0 || x >= world.getWidth() || y >= world.getHeight()) return false;
		return view[y][x].explored;
	}

	int x;
	int y;
	int viewDist;

  private:
	World& world;
	tilearray view;

};
