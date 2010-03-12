#include "ability.hh"
#include "actor.hh"

bool Ability_OpenDoor::operator()(Actor* self, Tile* target, bool force) {
	if (target->ch == '+') {
		if (self->confirmAction || force) {
			Tile tile = TileBuilder("Open door");
			*target = tile;
			self->confirmAction = false;
			self->msgs.push_back("You opened the door.");
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
		self->msgs.push_back("You closed the door.");
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

bool Ability_DetectEvil::operator()(Actor* self, Actor* target, bool force) {
	if (!(target->realType & EVIL_ACTORS)) return false;
	// Reveal possessed
	if (target->type == Actor::HUMAN) {
		self->msgs.push_back("The human is possessed!");
		target->msgs.push_back("You possessing the human is revealed!");
		target->type = Actor::POSSESSED;
		return true;
	}
	return false;
}

bool Ability_DetectDivinity::operator()(Actor* self, Actor* target, bool force) {
	if (!(target->realType & GOOD_ACTORS)) return false;
	// Reveal angels in disguise
	if (target->type == Actor::HUMAN) {
		self->msgs.push_back("The human is an angel in disguise!");
		target->msgs.push_back("Your disguise has been revealed!");
		target->type = Actor::CLOAKEDANGEL;
		return true;
	}
	return false;
}

bool Ability_TouchOfGod::operator()(Actor* self, Actor* target, bool force) {
	if (!(target->realType & EVIL_ACTORS)) return false;
	// Check form
	if (self->realType != self->type) {
		self->msgs.push_back("You must be in your true form to use Touch of God.");
		return false;
	}
	// Do some damage!
	int dmg = randint(5,7) + (self->realType == Actor::ARCHANGEL) ? randint(5,8) : 0;
	bool died = target->hurt(dmg);
	self->msgs.push_back(died ?
		std::string("You vanquished the ") + target->getTypeName(true) + "." :
		std::string("You punish the ") + target->getTypeName() + " (" + num2str(dmg) + ")."
		);
	target->msgs.push_back(died ?
		std::string("You fell to the attack of the ") + self->getTypeName(true) + "." :
		std::string("You were hurt by the ") + self->getTypeName() + " (" + num2str(dmg) + ")."
		);

	if (died) { self->addExp(target->realType == Actor::ARCHDEMON ? 2 : 1); self->killed_enemies++; }
	return true;
}

bool Ability_Bless::operator()(Actor* self, Actor* target, bool force) {
	if (target->realType != Actor::HUMAN) return false;
	// Check form
	if (self->realType != self->type) {
		self->msgs.push_back("You must be in your true form to use Bless.");
		return false;
	}
	if (!(target->realType & Actor::HUMAN) || target->blessed > 0 || target->possessed) return false;
	// Bless
	int blessing = randint(1,2) + (self->realType == Actor::ARCHANGEL) ? randint(2,3) : 0;
	target->blessed += blessing;
	self->msgs.push_back(std::string("You blessed the ") + target->getTypeName() + ".");
	self->addExp(1);
	self->killed_humans++; // Not really kills, but...
	return true;
}


bool Ability_HealSelf::operator()(Actor* self, bool force) {
	if (self->getExp() >= 2 && self->getHealth() < self->getMaxHealth()) {
		self->addExp(-2); self->hurt(-1);
		self->msgs.push_back("You feel a tiny bit better.");
	}
	return true;
}


bool Ability_Possess::operator()(Actor* self, Actor* target, bool force) {
	if (target->realType != Actor::HUMAN || self->possessing || target->possessed) return false;
	// Handle blessed humans
	if (target->blessed > 0) {
		self->hurt(target->blessed);
		self->msgs.push_back("You hurt yourself trying to possess a human blessed by an angel!");
		return true;
	}
	// Do possession
	self->msgs.push_back("You are now possessing the human.");
	self->possessing = target;
	target->possessed = self;
	self->type = Actor::HUMAN;
	self->addExp(1);
	// Move in-place
	self->getTilePtr()->actor = NULL;
	target->getTilePtr()->actor = self;
	self->x = target->x;
	self->y = target->y;
	return true;
}


bool Ability_DemonFire::operator()(Actor* self, Actor* target, bool force) {
	if (target->realType & EVIL_ACTORS) return false;
	// Handle no body
	if (!self->possessing && self->realType != Actor::ARCHDEMON) {
		self->msgs.push_back("You must be possessing a body to use demon fire.");
		return false;
	}
	// Reveal cloaked angel
	if (target->type == Actor::HUMAN && (target->realType & GOOD_ACTORS)) {
		self->msgs.push_back("The human is an angel in disguise!");
		target->type = Actor::CLOAKEDANGEL;
		return true;
	}
	// Reveal self when attacking angel
	if (self->type == Actor::HUMAN && self->possessing && target->realType & GOOD_ACTORS) {
		self->type = Actor::POSSESSED;
	}
	// Do fire
	int dmg = randint(5,6) + (self->realType == Actor::ARCHDEMON) ? randint(5,8) : 0;
	if (self->realType == Actor::IMP) dmg = randint(1,2);
	bool died = target->hurt(dmg);
	self->msgs.push_back(died ?
		std::string("You burnt the ") + target->getTypeName(true) + " to oblivion." :
		std::string("You burn the ") + target->getTypeName() + " (" + num2str(dmg) + ")."
		);
	target->msgs.push_back(died ?
		std::string("You fell to the attack of the ") + self->getTypeName(true) + "." :
		std::string("You were hurt by the ") + self->getTypeName() + " (" + num2str(dmg) + ")."
		);
	if (died) {
		int expadd = 1;
		if (target->realType == Actor::ANGEL) expadd++;
		if (target->realType == Actor::ARCHANGEL) expadd++;
		self->addExp(expadd);
		if (target->realType == Actor::HUMAN) self->killed_humans++;
		else self->killed_enemies++;
		self->hurt(-1); // Add some health (eating or something...)
	}
	return true;
}
