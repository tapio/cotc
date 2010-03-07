#pragma once

#include <boost/noncopyable.hpp>
#include <vector>

#include "console.hh"
#include "actor.hh"

class World: boost::noncopyable {
  public:
	World(): windowW(15), windowH(15) {
		scrX = COLS / 2 - windowW / 2;
		scrY = LINES / 2 - windowH / 2 - 1;
		worldwin = newwin(windowH, windowW, scrY, scrX);
	}
	~World() {
		delwin(worldwin);
	}

	void addActor(Actor& actor) { actors.push_back(&actor); }

	void draw(Actor& actor) {
		werase(worldwin);
		wcolor_set(worldwin, COLOR_GREEN, 0);
		box(worldwin, 0 , 0);
		for (Actors::const_iterator it = actors.begin(); it != actors.end(); it++) {
			wcolor_set(worldwin, (*it)->getColor(), 0);
			mvwaddch(worldwin, (*it)->y, (*it)->x, (*it)->getChar());
		}
		wrefresh(worldwin);
	}

  private:
	WINDOW* worldwin;
	int windowW;
	int windowH;
	int scrX;
	int scrY;
	typedef std::vector<Actor*> Actors;
	Actors actors;

};

