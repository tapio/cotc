#include "curses.h"

#include "common.hh"
#include "console.hh"
#include "actor.hh"
#include "world.hh"
#include "screens.hh"
#include "logger.hh"

#define KEY_ESCAPE 27


bool handleInput(Actor& pl) {
	int k = getch();

	if      (k == KEY_ESCAPE || k == 'q') return false;
	else if (k == KEY_LEFT   || k == '4') pl.move(-1,0);
	else if (k == KEY_RIGHT  || k == '6') pl.move(1,0);
	else if (k == KEY_UP     || k == '8') pl.move(0,-1);
	else if (k == KEY_DOWN   || k == '2') pl.move(0,1);
	else if (k == '7') pl.move(-1,-1);
	else if (k == '9') pl.move( 1,-1);
	else if (k == '1') pl.move(-1, 1);
	else if (k == '3') pl.move( 1, 1);
	else if (k == '?' || k == 'h') help();
	return true;
}


void mainLoop() {

	ConsoleWindow cons;
	frame();
	title();
	refresh();

	World world;
	Actor pl = Actor(world, Actor::ANGEL);
	pl.position(5,5);

	erase();
	frame();
	refresh();

	do {
		world.updateView(pl);
		world.draw(pl);
	} while(handleInput(pl));

}


int main() {

	mainLoop();

	return 0;
}

