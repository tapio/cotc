#include "rlutil.h"
#include "common.hh"
#include "console.hh"
#include "actor.hh"
#include "world.hh"

bool handleInput(Actor& actor) {
	if (kbhit()) {
		int k = rlutil::getkey();

		if (k == rlutil::KEY_ESCAPE) return false;
		else if (k == rlutil::KEY_LEFT || k == 'a') actor.move(-1,0);
		else if (k == rlutil::KEY_RIGHT || k == 'd') actor.move(1,0);
		else if (k == rlutil::KEY_UP || k == 'w') actor.move(0,-1);
		else if (k == rlutil::KEY_DOWN || k == 's') actor.move(0,1);
	}
	return true;
}


void mainLoop() {

	ConsoleWindow cons(40,24);
	World world(cons);
	Actor pl(Actor::ANGEL);
	pl.position(5,5);
	world.addActor(pl);

	do {

		world.draw(pl);
		cons.refresh();
	} while(handleInput(pl));

}


int main() {

	mainLoop();

	return 0;
}

