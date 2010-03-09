#include "ability.hh"


bool Ability_OpenDoor::operator()(Tile* target, bool force) {
	if (target->ch == '+') {
		Tile tile = TileBuilder("door_open");
		*target = tile;
		return true;
	}
	return false;
}
