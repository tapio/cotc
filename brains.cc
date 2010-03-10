#include "actor.hh"
#include "common.hh"
#include "logger.hh"

#define EVIL_ACTORS (IMP | DEMON | ARCHDEMON)
#define GOOD_ACTORS (ANGEL | ARCHANGEL)

Actor* Actor::getClosestActor(int types) {
	if (visible_actors.empty()) return NULL;
	if (types == 0) types = ALL;
	Actor* closest = NULL;
	int mindist = viewDist+1;
	for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it) {
		int dist = mindist;
		if ((*it)->type & types) {
			if (closest == NULL ||
			  (dist = distance2d(x, y, (*it)->x, (*it)->y)) < mindist) {
				closest = *it;
				mindist = dist;
			}
		}
	}
	return closest;
}

int Actor::countActors(int types) {
	if (visible_actors.empty()) return NULL;
	if (types == 0) types = ALL;
	int cnt = 0;
	for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it)
		if ((*it)->type & types) cnt++;
	return cnt;
}

// TODO: Make slide along the wall
void Actor::moveTowards(int tx, int ty) {
	int dx = 0, dy = 0;
	if (tx > x) dx = 1;
	if (tx < x) dx = -1;
	if (ty > y) dy = 1;
	if (ty < y) dy = -1;
	this->move(dx, dy);
}

void Actor::moveAway(int tx, int ty) {
	int invx = -(tx - x) + x;
	int invy = -(ty - y) + y;
	this->moveTowards(invx, invy);
}

void Actor::AI_human() {
	Actor* target = getClosestActor(DEMON | ARCHDEMON); // Imps are invisible
	if (target) { moveAway(target->x, target->y); return; }
	this->move(randint(-1,1), randint(-1,1));
}

void Actor::AI_demon() {
	int friendCount = countActors(EVIL_ACTORS);
	int enemyCount = countActors(GOOD_ACTORS);
	Actor* target = getClosestActor(GOOD_ACTORS);
	if (target) {
		// Only attack if superioir numbers
		if (friendCount > enemyCount) moveTowards(target->x, target->y);
		else moveAway(target->x, target->y);
		return;
	}
	target = getClosestActor(HUMAN);
	if (target) {
		moveTowards(target->x, target->y);
		return;
	}
	move(randint(-1,1), randint(-1,1));
}

void Actor::AI_angel() {
	Actor* target = getClosestActor(EVIL_ACTORS);
	if (target) {
		// Angels always attack fearlessly
		moveTowards(target->x, target->y);
		return;
	}
	move(randint(-1,1), randint(-1,1));
}
