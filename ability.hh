#pragma once

#include <vector>

#include "common.hh"

class Actor;

class Ability {
  public:
	enum Type { OPEN_DOOR } type;
	enum TargetType { SELF, ACTOR, WORLD } targetType;

	Ability(Type type): type(type) {
		switch (type) {
			case OPEN_DOOR: targetType = WORLD;
		}
	}

	bool use(Actor* target, bool force = false);
	bool use(Tile* target, bool force = false);

	std::string toString() const { return "Generic Ability"; }

  private:

};

typedef std::vector<Ability> Abilities ;
