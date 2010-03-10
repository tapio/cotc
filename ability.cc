#include "ability.hh"
#include "actor.hh"

bool Ability_OpenDoor::operator()(Actor* self, Tile* target, bool force) {
	if (target->ch == '+') {
		if (self->confirmAction || force) {
			Tile tile = TileBuilder("Open door");
			*target = tile;
			self->confirmAction = false;
			self->msgs.push_back("Door opened.");
		} else {
			self->confirmAction = true;
			self->msgs.push_back("Door is closed. Do you want to open it?");
		}
		return true;
	}
	return false;
}

bool Ability_CloseDoor::operator()(Actor* self, bool force) {
	Tile* target = self->getTilePtr();
	if (*target == TileBuilder("Open door")) {
		Tile tile = TileBuilder("Closed door");
		*target = tile;
		self->msgs.push_back("Door closed.");
		return true;
	}
	return false;
}

bool Ability_LookAt::operator()(Actor* self, Tile* target, bool force) {
	self->msgs.push_back(target->desc + ".");
	return true;
}

bool Ability_ConcealDivinity::operator()(Actor* self, bool force) {
	if (self->realType == self->type) {
		self->type = Actor::HUMAN;
		self->msgs.push_back("You took human form.");
	} else {
		self->type = self->realType;
		self->msgs.push_back("You took your real form.");
	}
	return true;
}
