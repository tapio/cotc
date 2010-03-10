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
	if (!(target->realType & EVIL_ACTORS)) return false;
	int dmg = randint(5,8) + (self->realType == Actor::ARCHANGEL) ? randint(5,8) : 0;
	bool died = target->hurt(dmg);
	self->msgs.push_back(died ?
		std::string("You vanquished the ") + target->getTypeName() + "." :
		std::string("You punish the ") + target->getTypeName() + " by " + num2str(dmg) + "."
		);
	if (died) self->addExp(target->realType == Actor::ARCHDEMON ? 2 : 1);
	return true;
}

bool Ability_Bless::operator()(Actor* self, Actor* target, bool force) {
	//if (self->realType != self->type) {
		//self->msgs.push_back("You must be in your true form to use Bless.");
		//return false;
	//}
	if (!(target->realType & Actor::HUMAN) || target->blessed > 0 || target->possessed) return false;
	int blessing = randint(2,3) + (self->realType == Actor::ARCHANGEL) ? randint(2,4) : 0;
	target->blessed += blessing;
	self->msgs.push_back(std::string("You blessed the ") + target->getTypeName() + ".");
	self->addExp(1);
	return true;
}


bool Ability_HealSelf::operator()(Actor* self, bool force) {
	if (self->getExp() > 1 && self->getHealth() < self->getMaxHealth()) {
		self->addExp(-1); self->hurt(-1);
	}
	return true;
}


bool Ability_Possess::operator()(Actor* self, Actor* target, bool force) {
	if (target->realType != Actor::HUMAN || self->possessing) return false;
	if (target->blessed > 0) {
		self->hurt(target->blessed);
		self->msgs.push_back("You hurt yourself trying to possess a human blessed by an angel.");
		return true;
	}
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
	if (!(target->realType & (Actor::HUMAN | GOOD_ACTORS))) return false;
	if (!self->possessing) {
		self->msgs.push_back("You must be possessing a body to use demon fire.");
		return false;
	}
	int dmg = randint(4,6) + (self->realType == Actor::ARCHDEMON) ? randint(5,8) : 0;
	if (self->realType == Actor::IMP) dmg = randint(1,2);
	bool died = target->hurt(dmg);
	self->msgs.push_back(died ?
		std::string("You burnt the ") + target->getTypeName() + " to oblivion." :
		std::string("You burn the ") + target->getTypeName() + " by " + num2str(dmg) + "."
		);
	if (died) {
		int expadd = 1;
		if (target->realType == Actor::ANGEL) expadd++;
		if (target->realType == Actor::ARCHANGEL) expadd++;
		self->addExp(expadd);
	}
	return true;
}
