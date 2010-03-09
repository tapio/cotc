#pragma once

#include "common.hh"

#define newAbility(a) dynamic_cast<Ability*>(new a)

class Actor;

class Ability {
  public:
	enum TargetType { SELF, ACTOR, WORLD } targetType;

	virtual bool operator()(Actor* target, bool force = false) { return false; }
	virtual bool operator()(Tile* target, bool force = false) { return false; }

	virtual std::string toString() const { return "Generic Ability"; }
};

typedef boost::ptr_vector<Ability> Abilities;

class Ability_OpenDoor: public Ability {
  public:
	bool operator()(Tile* target, bool force = false);
	std::string toString() const { return "Open door"; }
};
class Ability_CloseDoor: public Ability { };
class Ability_KickDoor: public Ability { };
class Ability_Talk: public Ability { };
