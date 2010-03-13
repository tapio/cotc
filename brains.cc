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
		if ((*it)->getType(this) & types) {
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
		if ((*it)->getType(this) & types) cnt++;
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
	Actor* target = getClosestActor(EVIL_ACTORS);
	if (target) {
		if (target->type != IMP || manhattan_dist(x,y,target->x,target->y) < 3) {
			moveAway(target->x, target->y);
			return;
		}
	}
	AI_generic();
}

void Actor::AI_demon() {
	int friendCount = countActors(EVIL_ACTORS);
	int enemyCount = countActors(GOOD_ACTORS);
	bool runaway = false;
	// Seek angels
	Actor* target = getClosestActor(GOOD_ACTORS);
	if (target) {
		// Only attack if superior numbers, right next to the enemy or archdemon
		if ((type == ARCHDEMON || possessing) && (friendCount > enemyCount || realType == ARCHDEMON
		  || max_axis_dist(x,y,target->x,target->y) <= 1)) {
			moveTowards(target->x, target->y);
			return;
		// Try to find a body
		} else if (!possessing) {
			targetx = target->x; targety = target->y;
			runaway = true;
		} else { moveAway(target->x, target->y); return; }
	}
	// Seek humans
	target = getClosestActor(HUMAN);
	if (target && (!runaway || distance2d(x,y,targetx,targety) >= distance2d(x,y,target->x,target->y))) {
		// Remember target
		targetx = target->x; targety = target->y;
		moveTowards(target->x, target->y);
		return;
	} else if (runaway) moveAway(targetx, targety);
	else AI_generic();
}

void Actor::AI_angel() {
	Actor* target = getClosestActor(EVIL_ACTORS);
	if (!target) target = getClosestActor(HUMAN);
	if (target && !target->blessed) { // Don't go after blessed people
		// Remember target
		targetx = target->x; targety = target->y;
		// Heal if needed
		if (getHealth() < 7 && getExp() > 1 && max_axis_dist(x,y,target->x,target->y) > 1) {
			Ability_HealSelf heal; heal(this);
			return;
		}
		// Decloak if near
		if (type != realType && max_axis_dist(x,y,target->x,target->y) <= 1) {
			Ability_ConcealDivinity decloak; decloak(this);
		}
		// Angels always attack fearlessly (or go blessing)
		moveTowards(target->x, target->y);
		return;
	}
	// Cloak
	if (type == realType) {
		Ability_ConcealDivinity cloak;
		cloak(this);
		return;
	}
	// Start search
	AI_generic();
}

void Actor::AI_generic() {
	// Check if there is a door underneath
	if (getTilePtr()->ch == '/' && randint(1,3) < 3) {
		Ability_CloseDoor close;
		close(this);
		return;
	}
	// Go towards a random target
	if (targetx && targety && max_axis_dist(x,y,targetx,targety) >= 2) {
		if (moveTowards(targetx,targety)) return;
	}
	// Move randomly
	if (randbool()) {
		int tx, ty; randdir(tx, ty);
		this->move(tx, ty);
	}
	// Acquire new destination
	if (randint(realType == HUMAN ? 10 : 2) == 0) {
		targetx = randint(1, world->getWidth()-2);
		targety = randint(1, world->getHeight()-2);
	}
}
