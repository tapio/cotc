#include "common.hh"

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
	int x, y;
	getyx(stdscr, y, x);
	x = COLS / 2 - str.length() / 2;
	mvaddstr(y, x, str.c_str());
}
