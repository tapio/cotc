#include "actor.hh"
#include "common.hh"
#include "logger.hh"


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

int Actor::countActors(int types) const {
	if (visible_actors.empty()) return NULL;
	if (types == 0) types = ALL;
	int cnt = 0;
	for (ActorPtrs::const_iterator it = visible_actors.begin(); it != visible_actors.end(); ++it)
		if ((*it)->type & types) cnt++;
	return cnt;
}

bool Actor::moveTowards(int tx, int ty) {
	int dx = 0, dy = 0;
	if (tx > x) dx = 1;
	if (tx < x) dx = -1;
	if (ty > y) dy = 1;
	if (ty < y) dy = -1;
	if (this->move(dx, dy)) return true;
	if (dx && dy) { // Try non-diagonal
		int tdx = dx, tdy = dy;
		if (randbool()) dx = 0; else dy = 0;
		if (this->move(dx,dy)) return true;
		if (dx) { dx = 0; dy = tdy; } else { dx = tdx; ty = tdy; }
		if (this->move(dx,dy)) return true;
	} else if (dx) { // Try diagonal
		dy = randdir();
		if (this->move(dx,dy)) return true;
		swapdir(dy);
		if (this->move(dx,dy)) return true;
	} else if (dy) { // Try diagonal
		dx = randdir();
		if (this->move(dx,dy)) return true;
		swapdir(dx);
		if (this->move(dx,dy)) return true;
	}
	return false;
}

void Actor::moveAway(int tx, int ty) {
	int invx = -(tx - x) + x;
	int invy = -(ty - y) + y;
	if (moveTowards(invx, invy)) return;
	// Try some random directions
	for (int i = 0; i < 4; i++) {
		randdir(tx, ty);
		if (moveTowards(tx,ty)) return;
	}
}

void Actor::AI_human() {
	Actor* target = getClosestActor(DEMON | ARCHDEMON); // Imps are invisible
	if (target) { moveAway(target->x, target->y); return; }
	if (targetx && targety && abs(targetx-x)+abs(targety-y) > 2) {
		if (moveTowards(targetx,targety)) return;
	}
	// Move randomly
	int tx, ty; randdir(tx, ty);
	this->move(tx, ty);
	// Acquire new destination
	if (randint(10) == 0) {
		targetx = randint(1, world->getWidth()-2);
		targety = randint(1, world->getHeight()-2);
	}
}

void Actor::AI_demon() {
	int friendCount = countActors(EVIL_ACTORS);
	int enemyCount = countActors(GOOD_ACTORS);
	Actor* target = getClosestActor(GOOD_ACTORS);
	if (target) {
		// Only attack if superior numbers
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
