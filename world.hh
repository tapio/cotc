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
			color_set((*it)->getColor(), 0);
			mvaddch((*it)->y, (*it)->x, (*it)->getChar());
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

