#pragma once

#include "world.hh"
#include "logger.hh"
#include "common.hh"

#define NO_AI false

class World;

class Actor {
  public:
	enum Type { HUMAN = 1, ANGEL = 2, ARCHANGEL = 4,
		IMP = 8, DEMON = 16, ARCHDEMON = 32, ALL = 63 } type;

	Actor(Type type, bool ai = true): type(type), x(), y(),
	  viewDist(10), useAI(ai), world(), moves() {
		switch (type) {
			case HUMAN: maxhealth = randint(3,7); break;
			case ANGEL: maxhealth = 16; break;
			case ARCHANGEL: maxhealth = 24; break;
			case IMP: maxhealth = randint(5,9); break;
			case DEMON: maxhealth = randint(13,16); break;
			case ARCHDEMON: maxhealth = randint(22,24); break;
			case ALL: break;
		}
		health = maxhealth*0.5;
	}

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
			case ALL: break;
		}
	}

	void idle() { moves++; }

	void move(int dx, int dy) {
		if (world->isFreeTile(x+dx, y+dy)) {
			world->getTilePtr(x, y)->actor = NULL;
			x+=dx;
			y+=dy;
			world->getTilePtr(x, y)->actor = this;
		}
		moves++;
	}

	bool position(int newx, int newy) {
		if (world->isFreeTile(newx, newy)) {
			world->getTilePtr(x, y)->actor = NULL;
			x = newx; y = newy;
			world->getTilePtr(x, y)->actor = this;
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
			case ALL:       return '\0';
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
			case ALL:       return -1;
		}
		return -1;
	}

	std::string getTypeName() const {
		switch(type) {
			case HUMAN:     return "Human";
			case ANGEL:     return "Angel";
			case ARCHANGEL: return "Archangel";
			case IMP:       return "Imp";
			case DEMON:     return "Demon";
			case ARCHDEMON: return "Archdemon";
			case ALL:       return "";
		}
		return "";
	}

	tilearray& getView() { return view; }

	const tilearray&  getConstView() const { return view; }

	bool hasExplored(int x, int y) const {
		if (x < 0 || y < 0 || x >= world->getWidth() || y >= world->getHeight()) return false;
		return view[y][x].explored;
	}

	void dumpDebugInfo() const {
		Logger log("ACTOR");
		log("Type: %s", getTypeName().c_str());
		log("Visible_actors: (count = %d)", visible_actors.size());
		for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it)
			log((*it)->getTypeName().c_str());
	}

	int x;
	int y;
	int viewDist;
	bool useAI;

	ActorPtrs visible_actors;

	int getExp() const { return exp; }
	int getHealth() const { return health; }
	int getMaxHealth() const { return maxhealth; }
	float getCond() const { return float(health) / maxhealth; }

  private:
	Actor* getClosestActor(int types = ALL);
	void moveTowards(int tx, int ty);
	void moveAway(int tx, int ty);

	void AI_angel();
	void AI_human();
	void AI_demon();

	WorldPtr world;
	tilearray view;

	int maxhealth;
	int health;
	int exp;
	unsigned long moves;
};
