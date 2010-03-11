#include <curses.h>
#include "common.hh"
#include "actor.hh"
#include "ability.hh"

void addflamestr(std::string str, bool letter = true) {
	static int flamecolors[] = { COLOR_RED, COLOR_YELLOW, COLOR_YELLOW+8 };
	int x,y;
	getyx(stdscr, y, x);
	x = COLS / 2 - str.length() / 2;
	move(y, x);
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '(') setColor(flamecolors[randint(3)]);
		else if (str[i] == ')') setColor(flamecolors[randint(3)]);
		else if ((str[i] == '_' || str[i] == '\'') && !letter) setColor(flamecolors[randint(3)]);
		else if (str[i] == '/' && !letter) setColor(flamecolors[randint(3)]);
		else if (str[i] == '\\' && !letter) setColor(flamecolors[randint(3)]);
		else setColor(COLOR_RED+8);
		addch(str[i]);
	}
}

int title() {
	clear();
	move(1,0);
	addflamestr("     (          )              (          )    )          \n", false);
	addflamestr("     )\\  (   ( /( (            )\\ )    ( /( ( /(    (     \n", false);
	addflamestr("   (((_) )\\  )\\()))\\ )     (  (()/(    )\\()))\\())  ))\\    \n", false);
	addflamestr("   )\\___((_)(_))/(()/(     )\\  /(_))  (_))/((_)\\  /((_)   \n", false);
	addflamestr("  ((/ __|(_)| |_  )(_))   ((_)()  _|  | |_ | |_))(_))     \n");
	addflamestr("   | (__ | ||  _|| || |  / _ \\ |  _|  |  _|| ' \\ / -_)    \n");
	addflamestr("    \\___||_| \\__| \\_, |  \\___/ |_|     \\__||_||_|\\___|    \n");
	addflamestr("   (               |__|(                              (   \n");
	addflamestr("   )\\                )\\ )   (     )             (   )\\ ) \n", false);
	addflamestr(" (((_)   ( '  (     (()/(  ))\\   (      (      ))\\ (()/( \n", false);
	addflamestr(" )\\_/_   )\\   )\\ )   ((_))/((_)  )\\  '  )\\ )  /((_) ((_))\n", false);
	addflamestr("((/ __| ((_) _(_((   _| |(_))  _((_))  _(_/( (_))   _| | \n");
	addflamestr(" | (__ / _ \\| ' \\) / _` |/ -_)| '  \\()| ' \\) / -_)/ _` | \n");
	addflamestr("  \\___|\\___/|_||_| \\__,_|\\___||_|_|_| |_||_| \\___|\\__,_| \n");
	move(LINES-7, 0);
	setColor(COLOR_GREEN+8);
	addcstr("[a] Join the Heavenly Host   "); addstr("\n");
	addcstr("[b] Join the forces of Hell  "); addstr("\n\n");
	addcstr("[c] Toggle default background"); addstr("\n");
	addcstr("[q] Quit                     ");
	setColor(COLOR_GREEN);
	box(stdscr, 0, 0);
	std::string vstr("Version: "); vstr += VERSION_STRING;
	mvaddstr(LINES-1, COLS-1-vstr.length(), vstr.c_str());
	setColor(COLOR_BLACK);
	while (true) {
		int k = getch();
		if (k == 'a' || k == 'A') return 1;
		if (k == 'b' || k == 'B') return 2;
		if (k == 'q' || k == 'Q' || k == KEY_ESCAPE) return 0;
		if (k == 'c' || k == 'C') { toggleDefaultColors(); return title(); }
	}
	flushinp();
}

bool winner(const Actor& pl) {
	if (pl.getExp() < 50) return false;
	if (!(pl.realType & (Actor::ARCHANGEL|Actor:: ARCHDEMON))) return false;
	clear();
	move(LINES/2,0);
	setColor(COLOR_BLUE+8);
	addcstr("You are victorious!");
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == KEY_ESCAPE || k == ' ' || k == KEY_ENTER || k == 'q' || k == 'Q') break;
	}
	flushinp();
	return true;
}

void death() {
	clear();
	move(LINES/2,0);
	setColor(COLOR_RED);
	addcstr("You failed!");
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == KEY_ESCAPE || k == ' ' || k == KEY_ENTER || k == 'q' || k == 'Q') break;
	}
	flushinp();
}

void frame(const Actor& pl, bool O = false) {
	int x = 2;
	int barwrap = 24;

	setColor(COLOR_BLACK);
	move(1,0);
	if (!O) {
		setColor(COLOR_GREEN);
		addstr("  Player - ");
		setColor(pl.getColor()); addstr(pl.getTypeName().c_str());
		setColor(COLOR_GREEN);
		addstr("\n\n");
		addstr("  Condition:");
	}
	// Health bar
	move(4,x);
	if (pl.getCond() < 0.2 || pl.getHealth() < 3) setColor(COLOR_RED+8);
	else if (pl.getCond() < 0.5) setColor(COLOR_YELLOW+8);
	else setColor(COLOR_GREEN+8);
	for (int i = 1; i <= pl.getMaxHealth(); ++i) {
		if ((i % barwrap) == 0) move(getcury(stdscr)+1, x);
		if (i > pl.getHealth()) setColor(COLOR_RED);
		echochar(i <= pl.getHealth() ? 'I' : '-');
	}
	// Experience
	move(getcury(stdscr)+2, x);
	if (!O) {
		std::string exptitle = "Experience:";
		if (pl.realType & EVIL_ACTORS) exptitle = "Chaos:";
		else if (pl.realType & GOOD_ACTORS) exptitle = "Divinity:";
		setColor(COLOR_GREEN);
		addstr(exptitle.c_str());
	}
	move(getcury(stdscr)+1, x);
	setColor(COLOR_BLUE+8);
	for (int i = 1; i <= pl.getNextExpLevel(); ++i) {
		if ((i % barwrap) == 0) move(getcury(stdscr)+1, x);
		echochar(i <= pl.getExp() ? 'I' : '-');
	}
	// Abilities
	move(getcury(stdscr)+2, x);
	if (!O) { setColor(COLOR_GREEN); addstr("Abilities:"); }
	move(getcury(stdscr)+1, x);
	setColor(COLOR_YELLOW);
	if (!O) {
		size_t i = 0;
		// Explicitly usable abilities
		for (Abilities::const_iterator it = pl.abilities.begin();
		  it != pl.abilities.end() && i < ability_keys.length(); ++it) {
			if (!it->hidden && !it->automatic) {
				addch('[');addch(ability_keys[i]);addstr("] "); addstr(it->toString().c_str());
				move(getcury(stdscr)+1, x);
				i++;
			}
		}
		// Automatic abilities
		for (Abilities::const_iterator it = pl.abilities.begin(); it != pl.abilities.end(); ++it) {
			if (!it->hidden && it->automatic) {
				addstr(" *  "); addstr(it->toString().c_str());
				move(getcury(stdscr)+1, x);
			}
		}

	}
	setColor(COLOR_RED);
	if (!pl.msgs.empty()) {
		move(LINES-2, 1);
		hline(' ', COLS-2);
		addcstr(pl.msgs.back().c_str());
	}
	if (!O) box(stdscr, 0, 0);
}

void help() {
	erase();
	setColor(COLOR_CYAN+8);
	box(stdscr, 0, 0);
	move(1,2);
	addcstr("HELP!");
	refresh();
	getch();
}


void msglog(const Actor& pl) {
	clear();
	move (1,0);
	setColor(COLOR_CYAN+8);
	addcstr("MESSAGE LOG");
	addch('\n');
	setColor(COLOR_GREEN);
	for (MsgBuffer::const_reverse_iterator it = pl.msgs.rbegin(); it != pl.msgs.rend(); ++it) {
		addcstr(*it); addch('\n');
	}
	setColor(COLOR_GREEN);
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == KEY_ESCAPE || k == ' ' || k == KEY_ENTER || k == 'q' || k == 'Q') break;
	}
	flushinp();
	clear();
}
