#pragma once

#include "rlutil.h"

class Actor {
  public:
	enum Type { HUMAN, ANGEL, ARCHANGEL, IMP, DEMON, ARCHDEMON } type;

	Actor(Type type): type(type) {};

	void move(int dx, int dy) {
		x+=dx;
		y+=dy;
	}

	void position(int newx, int newy) {
		x = newx;
		y = newy;
	}

	char getChar() const {
		switch(type) {
			case HUMAN:     return 'h';
			case ANGEL:     return 'a';
			case ARCHANGEL: return 'A';
			case IMP:       return 'i';
			case DEMON:     return 'd';
			case ARCHDEMON: return 'D';
		}
		return '\0';
	}

	int getColor() const {
		switch(type) {
			case HUMAN:     return rlutil::BROWN;
			case ANGEL:     return rlutil::GREY;
			case ARCHANGEL: return rlutil::WHITE;
			case IMP:       return rlutil::RED;
			case DEMON:     return rlutil::RED;
			case ARCHDEMON: return rlutil::RED;
		}
		return -1;
	}

	int x;
	int y;

  private:
};
