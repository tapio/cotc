#include "common.hh"

Tile TileBuilder(std::string type) {
	Tile tile;
	if (type == "Ground") tile = Tile('.', COLOR_GREEN, !BLOCKS);
	else if (type == "Grass") tile = Tile(',', COLOR_GREEN, !BLOCKS);
	else if (type == "Plaza") tile = Tile(':', COLOR_GREEN, !BLOCKS);
	else if (type == "Floor") tile = Tile('.', COLOR_YELLOW, !BLOCKS);
	else if (type == "Marble floor") tile = Tile(':', COLOR_YELLOW, !BLOCKS);
	else if (type == "Wall") tile = Tile('#', COLOR_CYAN, BLOCKS);
	else if (type == "Church") tile = Tile('T', COLOR_CYAN, BLOCKS);
	else if (type == "Tree") tile = Tile('T', COLOR_GREEN, BLOCKS);
	else if (type == "Statue") tile = Tile('S', COLOR_YELLOW, BLOCKS);
	else if (type == "Chair") tile = Tile('h', COLOR_YELLOW, !BLOCKS);
	else if (type == "Table") tile = Tile('O', COLOR_YELLOW, BLOCKSMOVE);
	else if (type == "Barrel") tile = Tile('o', COLOR_YELLOW, BLOCKSMOVE);
	else if (type == "Closet") tile = Tile('%', COLOR_YELLOW, BLOCKSMOVE);
	else if (type == "Locked closet") tile = Tile('%', COLOR_YELLOW, BLOCKSMOVE);
	else if (type == "Left bed") {
		tile = Tile('[', COLOR_YELLOW, BLOCKSMOVE);
		tile.desc = "Bed";
	} else if (type == "Right bed") {
		tile = Tile(']', COLOR_YELLOW, BLOCKSMOVE);
		tile.desc = "Bed";
	} else if (type == "Window") {
		tile = Tile('-', COLOR_BLUE, BLOCKS);
		tile.blocks_vision_dist = 1;
	} else if (type == "Open door") {
		tile = Tile('/', COLOR_YELLOW, !BLOCKS);
	} else if (type.find("door") != std::string::npos) {
		tile = Tile('+', COLOR_RED, BLOCKS);
	}
	if (tile.desc.empty()) tile.desc = type;
	return tile;
}


void setColor(WINDOW* scr, int color) {
	if (color < 0 || color > 15) return;
	if (color > 7) wattron(scr, A_BOLD);
	else wattroff(scr, A_BOLD);
	color = color % 8;
	wcolor_set(scr, color, 0);
}

void setColor(int color) { setColor(stdscr, color); }

void addnum(int num) {
	std::string stnum = num2str(num);
	for (size_t i = 0; i < stnum.length(); ++i) echochar(stnum[i]);
}

void addcstr(std::string str) {
	int x = COLS / 2 - str.length() / 2;
	mvaddstr(getcury(stdscr), x, str.c_str());
}

void addrstr(std::string str) {
	int x = COLS - str.length() - 2;
	mvaddstr(getcury(stdscr), x, str.c_str());
}

void addrowstr(const std::string str, size_t w) {
	size_t x = getcurx(stdscr);
	if (w <= 0) w = COLS - x - 2;
	std::string curword;
	size_t row_w = 0;
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == ' ') {
			size_t l = curword.length();
			if (row_w + 1 + l > w) { // Wrap
				move(getcury(stdscr)+1, x);
				row_w = l;
			} else { addch(' '); row_w += l + 1; } // Same row
			addstr(curword.c_str());
			curword = "";
		} else curword += str[i];
	}
	// Last word
	if (row_w + 1 + curword.length() > w) // Wrap
		move(getcury(stdscr)+1, x);
	else addch(' '); // Same row
	addstr(curword.c_str());
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
