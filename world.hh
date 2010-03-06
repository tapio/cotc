#pragma once

#include "rlutil.h"
#include "console.hh"
#include "actor.hh"

class World {
  public:
	World(ConsoleWindow& console): console(console), windowW(15), windowH(15), scrX(0), scrY(0) {

	}

	void addActor(Actor& actor) { actors.push_back(&actor); }

	void draw(Actor& actor) {
		for (Actors::const_iterator it = actors.begin(); it != actors.end(); it++) {
			console.put((*it)->getChar(), (*it)->x, (*it)->y, (*it)->getColor());
		}
	}

  private:
	ConsoleWindow& console;
	int windowW;
	int windowH;
	int scrX;
	int scrY;
	typedef std::vector<Actor*> Actors;
	Actors actors;

};

