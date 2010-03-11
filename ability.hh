#pragma once

#include "common.hh"

#define newAbility(a) dynamic_cast<Ability*>(new a)

const std::string ability_keys = "abcdefg";

class Actor;

class Ability {
  public:
	enum TargetType { SELF, ACTOR, WORLD } targetType;

	bool automatic;
	bool hidden;

	Ability(bool autom = true, bool hide = false): automatic(autom), hidden(hide) { }

	virtual bool operator()(Actor* target, bool force = false) { return false; }
	virtual bool operator()(Tile* target, bool force = false) { return false; }
	virtual bool operator()(Actor* self, Actor* target, bool force = false) { return false; }
	virtual bool operator()(Actor* self, Tile* target, bool force = false) { return false; }

	virtual std::string toString() const { return "Unknown Ability"; }
};

typedef boost::ptr_vector<Ability> Abilities;


struct Ability_OpenDoor: public Ability {
	bool operator()(Actor* self, Tile* target, bool force = false);
	std::string toString() const { return "Open door"; }
};

struct Ability_CloseDoor: public Ability {
	Ability_CloseDoor(): Ability(false) { }
	bool operator()(Actor* self, bool force = false);
	std::string toString() const { return "Close door"; }
};

struct Ability_LookAt: public Ability {
	Ability_LookAt(): Ability(true, true) { }
	bool operator()(Actor* self, Tile* target, bool force = false);
	std::string toString() const { return "Look at"; }
};

struct Ability_KickDoor: public Ability { };
struct Ability_Talk: public Ability { };

/// Section: Evil abilities

struct Ability_DetectDivinity: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Detect divinity"; }
};

struct Ability_Possess: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Possess soul"; }
};

struct Ability_DemonFire: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Demon fire"; }
};

/// Section: Angel abilities

struct Ability_DetectEvil: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Detect Evil"; }
};

struct Ability_TouchOfGod: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Touch of God"; }
};

struct Ability_Bless: public Ability {
	bool operator()(Actor* self, Actor* target, bool force = false);
	std::string toString() const { return "Bless"; }
};

struct Ability_ConcealDivinity: public Ability {
	Ability_ConcealDivinity(): Ability(false) { }
	bool operator()(Actor* self, bool force = false);
	std::string toString() const { return "Conceal divinity"; }
};

struct Ability_HealSelf: public Ability {
	Ability_HealSelf(): Ability(false) { }
	bool operator()(Actor* self, bool force = false);
	std::string toString() const { return "Heal self"; }
};
