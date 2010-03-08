#include <curses.h>
#include "common.hh"
#include "actor.hh"

void addflamestr(std::string str, bool letter = true) {
	static int flamecolors[] = { COLOR_RED, COLOR_YELLOW, COLOR_YELLOW+8 };
	int x,y;
	getyx(stdscr, y, x);
	x = COLS / 2 - str.length() / 2;
	move(y, x);
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '(') setColor(flamecolors[randint(3)]);
		else if (str[i] == ')') setColor(flamecolors[randint(3)]);
		else if (str[i] == '_' && !letter) setColor(flamecolors[randint(3)]);
		else if (str[i] == '/' && !letter) setColor(flamecolors[randint(3)]);
		else if (str[i] == '\\' && !letter) setColor(flamecolors[randint(3)]);
		else setColor(COLOR_GREEN);
		addch(str[i]);
	}
}

int title() {
	move(1,0);
	addflamestr("     (          )             (         )    )           \n", false);
	addflamestr("     )\\  (   ( /( (           )\\ )   ( /( ( /(    (      \n", false);
	addflamestr("   (((_) )\\  )\\()))\\ )    (  (()/(   )\\()))\\())  ))\\     \n", false);
	addflamestr("   )\\___((_)(_))/(()/(    )\\  /(_)) (_))/((_)\\  /((_)    \n", false);
	addflamestr("  ((/ __|(_)| |_  )(_))  ((_)(_) _| | |_ | |(_)(_))      \n");
	addflamestr("   | (__ | ||  _|| || | / _ \\ |  _| |  _|| ' \\ / -_)     \n");
	addflamestr("    \\___||_| \\__| \\_, | \\___/ |_|    \\__||_||_|\\___|     \n");
	addflamestr("   (              |__(                              (    \n");
	addflamestr("   )\\                )\\ )   (     )             (   )\\ ) \n", false);
	addflamestr(" (((_)   (    (     (()/(  ))\\   (      (      ))\\ (()/( \n", false);
	addflamestr(" )\\___   )\\   )\\ )   ((_))/((_)  )\\  '  )\\ )  /((_) ((_))\n", false);
	addflamestr("((/ __| ((_) _(_/(   _| |(_))  _((_))  _(_/( (_))   _| | \n");
	addflamestr(" | (__ / _ \\| ' \\))/ _` |/ -_)| '  \\()| ' \\))/ -_)/ _` | \n");
	addflamestr("  \\___|\\___/|_||_| \\__,_|\\___||_|_|_| |_||_| \\___|\\__,_| \n");
	move(LINES-8, 0);
	setColor(COLOR_GREEN+8);
	addcstr("[a] Join the Heavenly Host   "); addstr("\n");
	addcstr("[b] Join the forces of Hell  "); addstr("\n\n");
	addcstr("[c] Toggle default background"); addstr("\n");
	addcstr("[q] Quit                     ");
	setColor(COLOR_RED);
	box(stdscr, 0, 0);
	setColor(COLOR_BLACK);
	while (true) {
		int k = getch();
		if (k == 'a' || k == 'A') return 1;
		if (k == 'b' || k == 'B') return 2;
		if (k == 'q' || k == 'Q' || k == KEY_ESCAPE) return 0;
		if (k == 'c' || k == 'C') { toggleDefaultColors(); return title(); }
	}
}


void frame(const Actor& pl, bool O = false) {
	setColor(COLOR_BLACK);
	move(1,0);
	if (!O) {
		addstr("  Player - "); addstr(pl.getTypeName().c_str());
		addstr("\n\n");
		addstr("  Condition:\n");
		addstr("   IIIIIIIIIII");
		addstr("\n\n");
		addstr("  Experience:\n");
		addstr("   IIIIIIIIIII");
		addstr("\n\n");
		addstr("  Abilities:\n");
		addstr("  [o] Open door\n");
		addstr("  [k] Kick door\n");
		addstr("\n\n");
	}
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
