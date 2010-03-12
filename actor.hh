#pragma once

#include "world.hh"
#include "ability.hh"
#include "common.hh"

#define NO_AI false
#define EVIL_ACTORS (Actor::IMP | Actor::DEMON | Actor::ARCHDEMON | Actor::POSSESSED)
#define GOOD_ACTORS (Actor::ANGEL | Actor::ARCHANGEL | Actor::CLOAKEDANGEL)

class World;

class Actor: boost::noncopyable {
  public:
	enum Type { HUMAN = 1, ANGEL = 2, ARCHANGEL = 4, CLOAKEDANGEL = 8,
		IMP = 16, DEMON = 32, ARCHDEMON = 64, POSSESSED = 128, ALL = 255 } type;
	Type realType;

	Actor(Type type, bool ai = true): type(type), realType(type), x(), y(), targetx(), targety(),
	  viewDist(10), useAI(ai), confirmAction(false), exp(), blessed(), possessed(NULL), possessing(NULL),
	  forceRedrawUI(false), killed_enemies(), killed_humans(), moves(), msgs(20), world() {

		setInitialHealth();

		abilities.push_back(newAbility(Ability_OpenDoor));
		if (type & (GOOD_ACTORS)) abilities.push_back(newAbility(Ability_DetectEvil));
		if (type & (GOOD_ACTORS)) abilities.push_back(newAbility(Ability_TouchOfGod));
		if (type & (GOOD_ACTORS)) abilities.push_back(newAbility(Ability_Bless));
		if (type & (GOOD_ACTORS)) abilities.push_back(newAbility(Ability_HealSelf));
		if (type & (GOOD_ACTORS)) abilities.push_back(newAbility(Ability_ConcealDivinity));
		if (type & (EVIL_ACTORS)) abilities.push_back(newAbility(Ability_DetectDivinity));
		if (type & (EVIL_ACTORS)) abilities.push_back(newAbility(Ability_Possess));
		if (type & (EVIL_ACTORS)) abilities.push_back(newAbility(Ability_DemonFire));
		abilities.push_back(newAbility(Ability_CloseDoor));
	}

	~Actor() { abilities.clear(); }

	void setWorld(World* wd) {
		world = wd;
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
		switch (realType) {
			case HUMAN: AI_human(); break;
			case ANGEL: case ARCHANGEL: case CLOAKEDANGEL: AI_angel(); break;
			case IMP: case DEMON: case ARCHDEMON: case POSSESSED: AI_demon(); break;
			case ALL: break;
		}
	}

	bool possession() {
		int odds = 6;
		if (realType == DEMON) odds += 6;
		if (realType == ARCHDEMON) odds += 6;
		if (randint(odds) == 0) {
			this->move(randint(-1,1),randint(-1,1));
			return false;
		}
		return true;
	}

	void idle() { moves++; }

	bool move(int dx, int dy) {
		if (dx == 0 && dy == 0) return false;
		int tx = x+dx, ty = y+dy;
		if (world->isFreeTile(tx, ty)) {
			world->getTilePtr(x, y)->actor = NULL;
			x = tx; y = ty;
			world->getTilePtr(x, y)->actor = this;
			msgs.push_back(world->getTile(x, y).desc + ".");
			moves++;
			return true;
		} else {
			Actor* target = world->getTilePtr(tx, ty)->actor;
			for (Abilities::iterator it = abilities.begin(); it != abilities.end(); ++it) {
				if (target) {
					if ((*it)(this, target)) { moves++; return true; }
				} else {
					if ((*it)(this, world->getTilePtr(tx, ty))) { moves++; return true; }
				}
			}
		}
		return false;
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

	/// Function: getType
	/// Returns the apparant type taking into account askers side.
	Type getType(const Actor* asker = NULL) {
		if (asker && type == HUMAN) { // Handle same side
			if (asker->realType & GOOD_ACTORS && realType & GOOD_ACTORS) return realType;
			else if (asker->realType & EVIL_ACTORS && realType & EVIL_ACTORS) return realType;
		}
		// Normally, just return the apparant type
		return type;
	}

	char getChar() const {
		if (type & (HUMAN | POSSESSED | CLOAKEDANGEL)) return '@';
		switch(type) {
			case ANGEL:     return 'a';
			case ARCHANGEL: return 'A';
			case IMP:       return 'i';
			case DEMON:     return 'd';
			case ARCHDEMON: return 'D';
			default:        return '\0';
		}
	}

	int getColor(const Actor* asker = NULL) const {
		// Note: Bold colors are reserved for player
		if (asker && type == HUMAN) { // Handle same side
			if (asker->realType & GOOD_ACTORS && realType & GOOD_ACTORS) return COLOR_WHITE;
			else if (asker->realType & EVIL_ACTORS && realType & EVIL_ACTORS) return COLOR_RED;
		}
		if (type & HUMAN) return COLOR_YELLOW;
		else if (type & GOOD_ACTORS) return COLOR_WHITE;
		else if (type & EVIL_ACTORS) return COLOR_RED;
		return -1;
	}

	std::string getTypeName() const {
		switch(type) {
			case HUMAN:        return "Human";
			case CLOAKEDANGEL: return "Angel in disguise";
			case ANGEL:        return "Angel";
			case ARCHANGEL:    return "Archangel";
			case POSSESSED:    return "Possessed";
			case IMP:          return "Imp";
			case DEMON:        return "Demon";
			case ARCHDEMON:    return "Archdemon";
			case ALL:          return "";
		}
		return "";
	}

	void setInitialHealth() {
		switch (realType) {
			case HUMAN: maxhealth = randint(4,7); break;
			case ANGEL: maxhealth = 16; break;
			case ARCHANGEL: maxhealth = 28; break;
			case IMP: maxhealth = randint(8,9); break;
			case DEMON: maxhealth = randint(14,16); break;
			case ARCHDEMON: maxhealth = randint(24,27); break;
			default: break;
		}
		health = maxhealth;
	}

	tilearray& getView() { return view; }

	const tilearray&  getConstView() const { return view; }

	Tile* getTilePtr() { return world->getTilePtr(x,y); }

	const WorldPtr getConstWorldPtr() const { return world; }

	bool hasExplored(int x, int y) const {
		if (x < 0 || y < 0 || x >= world->getWidth() || y >= world->getHeight()) return false;
		return view[y][x].explored;
	}

	bool hurt(int dmg) {
		health -= dmg;
		dmg = clamp(dmg, 0, maxhealth);
		if (isDead()) {
			// Free possessed
			if (possessing) {
				possessing->possessed = NULL;
				possessing->x = x;
				possessing->y = y;
				possessing->getTilePtr()->actor = possessing;
			}
			return true;
		}
		return false;
	}

	void addExp(int howmuch) {
		exp += howmuch;
		if (exp >= getNextExpLevel()) {
			exp = getNextExpLevel();
			if (realType == IMP) evolve(DEMON);
			else if (realType == DEMON) evolve(ARCHDEMON);
			else if (realType == ANGEL) evolve(ARCHANGEL);
			else if (realType & (ARCHANGEL | ARCHDEMON)) {
				// Ascension, handled in winner();
			}
		}
	}

	void evolve(Type toType) {
		realType = toType;
		type = realType;
		msgs.push_back(std::string("You've been promoted to ") + getTypeName() + "!");
		if (possessing) { // Kill possessed
			possessing->hurt(1000);
			msgs.push_back("You're ascension destroyed your vessel.");
			killed_humans++;
		}
		setInitialHealth();
		exp = 0;
		forceRedrawUI = true;
	}

	int getNextExpLevel() const {
		if (realType == HUMAN);
		else if (realType == IMP) return 6;
		else if (realType == DEMON) return 10;
		else if (realType == ANGEL) return 16;
		else if (realType == ARCHANGEL) return 16;
		else if (realType == ARCHDEMON) return 16;
		return 1000;
	}

	bool isDead() const { return health <= 0; }

	void dumpDebugInfo() const {
		Logger log("ACTOR");
		log("Type: %s", getTypeName().c_str());
		log("Visible_actors: (count = %d)", visible_actors.size());
		for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it)
			log((*it)->getTypeName().c_str());
	}

	int x;
	int y;
	int targetx;
	int targety;
	int viewDist;
	bool useAI;
	bool confirmAction;
	int exp;
	int blessed;
	Actor* possessed;
	Actor* possessing;
	bool forceRedrawUI;

	int killed_enemies;
	int killed_humans;
	int moves;

	ActorPtrs visible_actors;
	Abilities abilities;
	MsgBuffer msgs;

	int getExp() const { return exp; }
	int getHealth() const { return health; }
	int getMaxHealth() const { return maxhealth; }
	float getCond() const { return float(health) / maxhealth; }

  private:
	Actor* getClosestActor(int types = ALL);
	int countActors(int types = ALL) const;
	bool moveTowards(int tx, int ty);
	void moveAway(int tx, int ty);

	void AI_angel();
	void AI_human();
	void AI_demon();
	void AI_generic();

	WorldPtr world;
	tilearray view;

	int maxhealth;
	int health;
};
