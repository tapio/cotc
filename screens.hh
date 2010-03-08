#include <curses.h>
#include "common.hh"

void addcstr(std::string str) {
	int x, y;
	getyx(stdscr, y, x);
	x = COLS / 2 - str.length() / 2;
	mvaddstr(y, x, str.c_str());
}

int title() {
	move(1,0);
	addcstr("     (          )             (         )    )           \n");
	addcstr("     )\\  (   ( /( (           )\\ )   ( /( ( /(    (      \n");
	addcstr("   (((_) )\\  )\\()))\\ )    (  (()/(   )\\()))\\())  ))\\     \n");
	addcstr("   )\\___((_)(_))/(()/(    )\\  /(_)) (_))/((_)\\  /((_)    \n");
	addcstr("  ((/ __|(_)| |_  )(_))  ((_)(_) _| | |_ | |(_)(_))      \n");
	addcstr("   | (__ | ||  _|| || | / _ \\ |  _| |  _|| ' \\ / -_)     \n");
	addcstr("    \\___||_| \\__| \\_, | \\___/ |_|    \\__||_||_|\\___|     \n");
	addcstr("   (              |__(                              (    \n");
	addcstr("   )\\                )\\ )   (     )             (   )\\ ) \n");
	addcstr(" (((_)   (    (     (()/(  ))\\   (      (      ))\\ (()/( \n");
	addcstr(" )\\___   )\\   )\\ )   ((_))/((_)  )\\  '  )\\ )  /((_) ((_))\n");
	addcstr("((/ __| ((_) _(_/(   _| |(_))  _((_))  _(_/( (_))   _| | \n");
	addcstr(" | (__ / _ \\| ' \\))/ _` |/ -_)| '  \\()| ' \\))/ -_)/ _` | \n");
	addcstr("  \\___|\\___/|_||_| \\__,_|\\___||_|_|_| |_||_| \\___|\\__,_| \n");
	move(LINES-7, 0);
	addcstr("[a] Join the Heavenly Host "); addstr("\n\n");
	addcstr("[b] Join the forces of Hell"); addstr("\n\n");
	addcstr("[q] Quit                   ");
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == 'a') return 1;
		if (k == 'b') return 2;
		if (k == 'q' || k == KEY_ESCAPE) return 0;
	}
}


void frame(bool O = false) {
	if (O) { move(0,1); hline(0, COLS-2); }
	else box(stdscr, 0, 0);
}

void help() {
	erase();
	box(stdscr, 0, 0);
	move(0,2);
	addcstr("HELP!");
	refresh();
	getch();
}
