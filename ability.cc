#include "ability.hh"

bool Ability::use(Actor* target, bool force) {
	if (targetType != ACTOR && targetType != SELF) return false;
	return false;
}

bool Ability::use(Tile* target, bool force) {
	if (targetType != WORLD) return false;
	switch (type) {
		case OPEN_DOOR:
			if (target->ch == '+') {
				*target = TileBuilder("door_open");
				return true;
			}
			break;
	}
	return false;

}
