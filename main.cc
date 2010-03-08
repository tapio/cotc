#include <curses.h>
#include <boost/shared_ptr.hpp>

#include "common.hh"
#include "console.hh"
#include "actor.hh"
#include "world.hh"
#include "screens.hh"
#include "logger.hh"

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
	else if (k == '5') pl.idle();
	else if (k == '?' || k == 'h') { help(); frame(pl); }

	// Cheats
	else if (k == KEY_F(5)) pl.type = Actor::HUMAN;
	else if (k == KEY_F(6)) pl.type = Actor::ANGEL;
	else if (k == KEY_F(7)) pl.type = Actor::DEMON;
	else if (k == KEY_F(4)) pl.viewDist = 100;
	return true;
}


void mainLoop() {

	ConsoleWindow cons;
	int abc = title();
	if (abc == 0) return;

	boost::shared_ptr<World> world(new World());
	Actor& pl(world->addActor(new Actor(abc == 1 ? Actor::ANGEL : Actor::IMP, NO_AI)));
	pl.position(abc == 1 ? world->getWidth()-3 : 2, world->getHeight() / 2);

	// Actors
	for (int i = 0; i < 30; i++) {
		Actor& a(world->addActor(new Actor(Actor::HUMAN)));
		while (!a.position(randint(world->getWidth()),
		                   randint(world->getHeight())));
	}
	for (int i = 0; i < 20; i++) {
		Actor& a(world->addActor(new Actor(Actor::IMP)));
		while (!a.position(randint(world->getWidth()),
		                   randint(world->getHeight())));
	}
	for (int i = 0; i < 20; i++) {
		Actor& a(world->addActor(new Actor(Actor::ANGEL)));
		while (!a.position(randint(world->getWidth()),
		                   randint(world->getHeight())));
	}

	erase();
	frame(pl);
	refresh();

	do {
		frame(pl, true);
		refresh();
		world->update();
		world->draw(pl);
	} while(handleInput(pl));

}


int main() {

	mainLoop();

	return 0;
}

