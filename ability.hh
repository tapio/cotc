#pragma once

#include "common.hh"

#define newAbility(a) dynamic_cast<Ability*>(new a)

const std::string ability_keys = "abcdefg";

class Actor;

class Ability {
  public:
	enum TargetType { SELF, ACTOR, WORLD } targetType;

	bool hidden;

	Ability(bool hide = false): hidden(hide) { }

	virtual bool operator()(Actor* target, bool force = false) { return false; }
	virtual bool operator()(Actor* self, Tile* target, bool force = false) { return false; }
	virtual bool operator()(Tile* target, bool force = false) { return false; }

	virtual std::string toString() const { return "Unknown Ability"; }
};

typedef boost::ptr_vector<Ability> Abilities;



struct Ability_OpenDoor: public Ability {
	Ability_OpenDoor(): Ability(true) { }
	bool operator()(Actor* self, Tile* target, bool force = false);
};
/*
class Ability_CloseDoor: public Ability {

	std::string toString() const { return "Close door"; }
};

class Ability_LookAt: public Ability {
	Ability_LookAt(): hidden(true) { }
};
*/
class Ability_KickDoor: public Ability { };
class Ability_Talk: public Ability { };

class Ability_Possess: public Ability {
	std::string toString() const { return "Possess soul"; }
};

class Ability_DemonFire: public Ability {
	std::string toString() const { return "Demon fire"; }
};

class Ability_ConcealDivinity: public Ability {
	bool operator()(Actor* self, bool force = false);
	std::string toString() const { return "Conceal divinity"; }
};
