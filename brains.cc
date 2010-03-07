#include "actor.hh"
#include "common.hh"
#include "logger.hh"

Actor* Actor::getClosestActor(int types) {
	if (visible_actors.empty()) return NULL;
	if (types == 0) types = ALL;
	Actor* closest = NULL;
	int mindist = viewDist+1;
	for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it) {
		int dist;
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
	Actor* target = getClosestActor(IMP | DEMON | ARCHDEMON);
	if (target) { moveAway(target->x, target->y); return; }
	this->move(randint(-1,1), randint(-1,1));
}

void Actor::AI_demon() {
	Actor* target = getClosestActor(ANGEL | ARCHANGEL);
	if (target) {
		moveTowards(target->x, target->y);
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
	Actor* target = getClosestActor(IMP | DEMON | ARCHDEMON);
	if (target) {
		moveTowards(target->x, target->y);
		return;
	}
	move(randint(-1,1), randint(-1,1));
}
