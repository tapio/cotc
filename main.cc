#include <curses.h>
#include <ctime>

#include "version.h"
#ifndef VERSION_STRING
	#define VERSION_STRING "unknown"
#endif

#include "common.hh"
#include "console.hh"
#include "actor.hh"
#include "world.hh"
#include "screens.hh"

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
	else if (k == '?' || k == 'h' || k == KEY_F(1)) { help(); frame(pl); }
	// Abilities
	else if (ability_keys.find(k) != std::string::npos) {
		size_t i = 0;
		// Find the ability
		for (Abilities::iterator it = pl.abilities.begin();
		  it != pl.abilities.end() && i < ability_keys.length(); ++it) {
			if (it->hidden || it->automatic) continue; // Only explicitly usable skills have a key
			if (ability_keys[i] == k) {
				(*it)(&pl); break; // Do action
			}
			i++;
		}
	}

	// Cheats
	else if (k == KEY_F(5)) pl.type = Actor::HUMAN;
	else if (k == KEY_F(6)) pl.type = Actor::ANGEL;
	else if (k == KEY_F(7)) pl.type = Actor::DEMON;
	else if (k == KEY_F(4)) pl.viewDist = 100;

	flushinp();
	return true;
}


void mainLoop() {

	ConsoleWindow cons;
	int abc = title();
	if (abc == 0) return;

	boost::scoped_ptr<World> world(new World());
	Actor& pl(world->addActor(new Actor(abc == 1 ? Actor::ANGEL : Actor::IMP, NO_AI)));
	pl.position(abc == 1 ? world->getWidth()-3 : 2, world->getHeight() / 2);
	pl.abilities.push_back(newAbility(Ability_LookAt));

	// Actors
	Actor::Type actor_types[] = {
		Actor::HUMAN, Actor::HUMAN, Actor::HUMAN, Actor::HUMAN,
		Actor::HUMAN, Actor::HUMAN, Actor::HUMAN, Actor::HUMAN,
		Actor::IMP,   Actor::IMP,   Actor::DEMON, Actor::ANGEL
	};
	for (int i = 0; i < 100; i++) {
		Actor& a(world->addActor(new Actor(actor_types[randint(12)])));
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


int main(int argc, char* argv[]) {
	// Process commandline
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			std::string arg(argv[i]);
			if (arg == "-v" || arg == "--version") {
				std::cout << "Version: " << VERSION_STRING << std::endl;
				exit(0);
			} else if (arg == "-h" || arg == "--help") {
				std::cout << "Usage: " << argv[0] << " [-v|--version] [-h|--help]" << std::endl;
				exit(0);
			} else {
				std::cout << "Unrecognized parameter: " << arg << std::endl;
				exit(-1);
			}
		}
	}

	srand(time(NULL)); // Randomize RNG

	mainLoop();

	return 0;
}

