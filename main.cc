#include "curses.h"

#include "rlutil.h"
#include "common.hh"
#include "console.hh"
#include "actor.hh"
#include "world.hh"
#include "screens.hh"

bool handleInput(Actor& actor) {
	int k = getch();

	if (k == 'q') return false;
	else if (k == KEY_LEFT  || k == 'a') actor.move(-1,0);
	else if (k == KEY_RIGHT || k == 'd') actor.move(1,0);
	else if (k == KEY_UP    || k == 'w') actor.move(0,-1);
	else if (k == KEY_DOWN  || k == 's') actor.move(0,1);
	return true;
}


void mainLoop() {

	ConsoleWindow cons;
	frame();
	title();
	refresh();

	World world;
	Actor pl(Actor::ANGEL);
	pl.position(5,5);
	world.addActor(pl);

	erase();
	frame();
	refresh();

	do {
		world.draw(pl);
	} while(handleInput(pl));

}


int main() {

	mainLoop();

	return 0;
}

