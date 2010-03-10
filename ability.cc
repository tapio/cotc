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

bool Ability_TouchOfGod::operator()(Actor* self, Actor* target, bool force) {
	if (self->realType != self->type) {
		self->msgs.push_back("You must be in your true form to use Touch of God.");
		return false;
	}
	if (!(target->type & EVIL_ACTORS)) return false;
	int dmg = randint(5,8) + (self->realType == Actor::ARCHANGEL) ? randint(5,8) : 0;
	bool died = target->hurt(dmg);
	self->msgs.push_back(died ?
		std::string("You vanquished the ") + target->getTypeName() + "." :
		std::string("You punish the ") + target->getTypeName() + " by " + num2str(dmg) + "."
		);
	if (died) self->exp++;
	return true;
}

bool Ability_Bless::operator()(Actor* self, Actor* target, bool force) {
	//if (self->realType != self->type) {
		//self->msgs.push_back("You must be in your true form to use Bless.");
		//return false;
	//}
	if (!(target->type & Actor::HUMAN) || target->blessed > 0) return false;
	int blessing = randint(5,8) + (self->realType == Actor::ARCHANGEL) ? randint(5,8) : 0;
	target->blessed += blessing;
	self->msgs.push_back(std::string("You blessed the ") + target->getTypeName() + ".");
	self->exp++;
	return true;
}


bool Ability_HealSelf::operator()(Actor* self, bool force) {
	if (self->getExp() > 1 && self->getHealth() < self->getMaxHealth()) {
		self->exp--; self->hurt(-1);
	}
	return true;
}
