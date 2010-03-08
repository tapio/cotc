#include "common.hh"

Tile TileBuilder(std::string type) {
	Tile tile;
	if (type == "window") {
		tile = Tile('*', COLOR_BLUE, BLOCKS);
		tile.blocks_vision_dist = 2;
	} else if (type == "door_open") {
		tile = Tile('/', COLOR_YELLOW, !BLOCKS);
	} else if (type.substr(0,4) == "door") {
		tile = Tile('+', COLOR_RED, BLOCKS);
	}
	return tile;
}


void setColor(WINDOW* scr, int color) {
	if (color < 0 || color > 15) return;
	if (color > 7) wattron(scr, A_BOLD);
	else wattroff(scr, A_BOLD);
	color = color % 8;
	wcolor_set(scr, color, 0);
}

void setColor(int color) {
	setColor(stdscr, color);
}

void addcstr(std::string str) {
	int x = COLS / 2 - str.length() / 2;
	mvaddstr(getcury(stdscr), x, str.c_str());
}

void box2(int x, int y, int w, int h) {
	move(y,x);
	echochar(ACS_ULCORNER);
	for (int i = x; i < x+w-2; ++i) echochar(ACS_HLINE);
	echochar(ACS_URCORNER);

	for (int j = y+1; j < y+h-1; ++j) {
		move(j,x); echochar(ACS_VLINE);
		move(j,x+w-1); echochar(ACS_VLINE);
	}

	move(y+h-1,x);
	echochar(ACS_LLCORNER);
	for (int i = x; i < x+w-2; ++i) echochar(ACS_HLINE);
	echochar(ACS_LRCORNER);
}

bool toggleDefaultColors() {
	static bool use_defaults = true;
	use_defaults = !use_defaults;
	if (use_defaults) use_default_colors();
	else assume_default_colors(0,0);
	clear();
	return use_defaults;
}
