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
	else if (k == '+' || k == '?' || k == KEY_F(1)) { help(); frame(pl); }
	else if (k == 'm' || k == 'l' || k == KEY_F(2)) { msglog(pl); frame(pl); }

	else if (!pl.possessing || pl.possession()) {
		if (k == ' ' || k == '5') pl.idle();
		else if (k == KEY_LEFT   || k == '4' || k == 'g' || k == 'G') pl.move(-1,0);
		else if (k == KEY_RIGHT  || k == '6' || k == 'j' || k == 'J') pl.move(1,0);
		else if (k == KEY_UP     || k == '8' || k == 'y' || k == 'Y') pl.move(0,-1);
		else if (k == KEY_DOWN   || k == '2' || k == 'h' || k == 'H') pl.move(0,1);
		else if (k == '7' || k == 't' || k == 'T') pl.move(-1,-1);
		else if (k == '9' || k == 'u' || k == 'U') pl.move( 1,-1);
		else if (k == '1' || k == 'b' || k == 'B') pl.move(-1, 1);
		else if (k == '3' || k == 'n' || k == 'N') pl.move( 1, 1);
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
		/*
		else if (k == KEY_F(4)) pl.viewDist = 100;
		else if (k == KEY_F(5)) pl.type = Actor::HUMAN;
		else if (k == KEY_F(6)) pl.type = Actor::ANGEL;
		else if (k == KEY_F(7)) pl.type = Actor::DEMON;
		else if (k == KEY_F(8)) pl.addExp(1);
		else if (k == KEY_F(9)) pl.hurt(1);
		*/
	} else pl.msgs.push_back("The possessed soul revolts.");

	flushinp();
	return true;
}


bool mainLoop() {

	int abc = title();
	if (abc == 0) return false; // Quit game

	boost::scoped_ptr<World> world(new World());
	Actor& pl(world->addActor(new Actor(abc == 1 ? Actor::ANGEL : Actor::IMP, NO_AI)));
	pl.position(abc == 1 ? world->getWidth()-3 : 2, world->getHeight() / 2);
	pl.abilities.push_back(newAbility(Ability_LookAt));
	pl.msgs.push_back("Hit '?' for help.");
	pl.msgs.push_back("Hit 'm' to see the message log.");
	pl.msgs.push_back("Welcome to the City of the Condemned.");

	// Actors
	Actor::Type actor_types[] = {
		Actor::HUMAN, Actor::HUMAN, Actor::HUMAN, Actor::HUMAN,
		Actor::HUMAN, Actor::HUMAN, Actor::HUMAN, Actor::HUMAN,
		Actor::HUMAN, Actor::HUMAN, Actor::HUMAN, Actor::HUMAN,
		Actor::HUMAN, Actor::ANGEL, Actor::DEMON, Actor::IMP,
		Actor::IMP,   Actor::IMP,   Actor::IMP,   Actor::IMP
	};
	for (int i = 0; i < 200; i++) {
		Actor& a(world->addActor(new Actor(actor_types[randint(20)])));
		while (!a.position(randint(world->getWidth()),
		                   randint(world->getHeight())));
	}
	world->updateActorsMeta();

	erase();
	frame(pl);
	refresh();

	do {
		frame(pl, !pl.forceRedrawUI); pl.forceRedrawUI = false;
		refresh();
		world->update();
		world->draw(pl);
	} while(!checkWinLose(pl) && handleInput(pl) && !checkWinLose(pl));

	return true; // Come back to main loop for the title screen
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

	{
		ConsoleWindow cons;
		while (mainLoop());
	}
	return 0;
}

