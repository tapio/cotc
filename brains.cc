#include "actor.hh"
#include "common.hh"

void Actor::AI_human() {
	move(randint(-1,1), randint(-1,1));
}

void Actor::AI_demon() {
	AI_human();
}

void Actor::AI_angel() {
	AI_demon();
}
