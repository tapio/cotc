#include <curses.h>

void addcstr(std::string str) {
	int x, y;
	getyx(stdscr, y, x);
	x = COLS / 2 - str.length() / 2;
	mvaddstr(y, x, str.c_str());
}

void title() {
	//addstr("        ___  ____  ____  _  _\n");
	//addstr("       / __)(_  _)(_  _)( \/ )\n");
	//addstr("      ( (__  _)(_   )(   \  /\n");
	//addstr("       \___)(____) (__)  (__)\n");
	//addstr("   _____  ____    ____  _   _  ____\n");
	//addstr("  (  _  )( ___)  (_  _)( )_( )( ___)\n");
	//addstr("   )(_)(  )__)     )(   ) _ (  )__)\n");
	//addstr("  (_____)(__)     (__) (_) (_)(____)\n");
	//addstr(" ____    __    __  __  _  _  ____  ____\n");
	//addstr("(  _ \  /__\  (  \/  )( \( )( ___)(  _ \ \n");
	//addstr(" )(_) )/(__)\  )    (  )  (  )__)  )(_) )\n");
	//addstr("(____/(__)(__)(_/\/\_)(_)\_)(____)(____/ \n");
	//getch();
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
