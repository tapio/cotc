#include "curses.h"

#include "rlutil.h"
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
	else if (k == KEY_LEFT   || k == 'a') pl.move(-1,0);
	else if (k == KEY_RIGHT  || k == 'd') pl.move(1,0);
	else if (k == KEY_UP     || k == 'w') pl.move(0,-1);
	else if (k == KEY_DOWN   || k == 's') pl.move(0,1);
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
		world.draw(pl);
	} while(handleInput(pl));

}


int main() {

	mainLoop();

	return 0;
}

