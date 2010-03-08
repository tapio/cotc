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
