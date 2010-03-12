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

void msglog(const Actor& pl) {
	clear();
	move (1,0);
	setColor(COLOR_CYAN+8);
	addcstr("MESSAGE LOG");
	addch('\n');
	setColor(COLOR_GREEN);
	int mcnt = 1;
	for (MsgBuffer::const_reverse_iterator it = pl.msgs.rbegin(); it != pl.msgs.rend(); ++mcnt) {
		// HACK: Filter tile descriptions by length, delete short ones
		// No hack: too few messages as the short ones still are there - erasing requires non-const
		//if (it->length() >= 15) { addcstr(*it); addch('\n'); }
		//else if (mcnt >= 3 && it->length() < 15) { it = pl.msgs.erase(it); continue; }
		addcstr(*it); addch('\n');
		++it;
	}
	setColor(COLOR_GREEN);
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == KEY_ESCAPE || k == ' ' || k == KEY_ENTER
		  || k == 'q' || k == 'Q' || k == 'm' || k == 'M') break;
	}
	flushinp();
	clear();
}

bool checkWinLose(const Actor& pl) {
	// Here be ending conditions
	WorldPtr wptr = pl.getConstWorldPtr();
	std::string endst;
	std::string tempst;
	bool win = false;
	// Death
	if (pl.isDead()) { endst = "You were obliterated!"; win = false; }
	// Angel conditions
	else if (pl.realType & GOOD_ACTORS) {
		if (wptr->humans <= 0) {
			endst = "All humans were killed!"; win = false;
		} else if (wptr->blessed > wptr->alltime_humans / 2) {
			endst = "Over half of the humans are blessed!"; win = true;
		} else if (wptr->demons <= 0) {
			endst = "All Evil was vanquished!"; win = true;
		} else if (pl.realType == Actor::ARCHANGEL && pl.getExp() >= pl.getNextExpLevel()) {
			endst = "You became a Seraph, guardian of God's throne!"; win = true;
		} else return false;
	} else if (pl.realType & EVIL_ACTORS) {
		if (wptr->humans <= 0) {
			endst = "All humans were killed!"; win = true;
		} else if (wptr->blessed > wptr->alltime_humans / 2) {
			endst = "Over half of the humans are blessed!"; win = false;
		} else if (wptr->angels <= 0) {
			endst = "All angels were vanquished!"; win = true;
		} else if (pl.realType == Actor::ARCHDEMON && pl.getExp() >= pl.getNextExpLevel()) {
			endst = "You became the Devil's right-hand demon!"; win = true;
		} else return false;
	} else return false;
	clear();
	// Titles
	move(1,0);
	setColor((win ? COLOR_GREEN : COLOR_RED));
	addcstr(win ? "VICTORY" : "DEFEAT"); addch('\n'); addch('\n');
	addcstr(endst); addch('\n'); addch('\n');
	// World stats
	// TODO
	// Deeds
	addcstr("   Your deeds:   "); addch('\n');
	tempst = num2str(pl.killed_enemies) + std::string(pl.realType & EVIL_ACTORS ?
		" angels crushed" : " demons crushed");
	addcstr(tempst); addch('\n');
	tempst = num2str(pl.killed_humans) + std::string(pl.realType & EVIL_ACTORS ?
		" humans killed" : " humans blessed");
	addcstr(tempst); addch('\n');
	tempst = num2str(pl.moves) + " moves made    ";
	addcstr(tempst); addch('\n');
	// Menu
	move(LINES-5, 0);
	setColor(COLOR_YELLOW);
	addcstr("[m] See last messages   "); addch('\n');
	addcstr("[q] Quit to title screen");
	// Frame
	setColor((win ? COLOR_GREEN : COLOR_RED));
	box(stdscr, 0, 0);
	while (true) {
		int k = getch();
		if (k == 'm' || k == 'M') { msglog(pl); return true; }
		else if (k == KEY_ESCAPE || k == ' ' || k == KEY_ENTER || k == 'q' || k == 'Q') break;
	}
	flushinp();
	return true;
}

void frame(const Actor& pl, bool O = false) {
	if (!O) erase();
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
	// Messages
	setColor(COLOR_RED+8);
	move(LINES-4, 1);
	int cnt = 0;
	for (MsgBuffer::const_reverse_iterator it = pl.msgs.rbegin(); it != pl.msgs.rend() && cnt < 3; ++it, ++cnt) {
		hline(' ', COLS-2);
		addrstr(it->c_str());
		move(getcury(stdscr)+1, 1);
		if (cnt == 0) setColor(COLOR_RED);
	}
	// World status
	setColor(COLOR_GREEN);
	move(LINES - 5, x);         addstr("Humans:       ");
	move(getcury(stdscr)+1, x); addstr("Blessed:      ");
	move(getcury(stdscr)+1, x); addstr("Angels:       ");
	move(getcury(stdscr)+1, x); addstr("Demons:       ");
	const WorldPtr wptr(pl.getConstWorldPtr());
	if (wptr) {
		move(LINES - 5, x + 9);         addnum(wptr->humans);
		move(getcury(stdscr)+1, x + 9); addnum(wptr->blessed);
		move(getcury(stdscr)+1, x + 9); addnum(wptr->angels);
		move(getcury(stdscr)+1, x + 9); addnum(wptr->demons);
	}
	// Frame
	setColor(COLOR_RED);
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
