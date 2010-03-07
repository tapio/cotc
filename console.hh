#pragma once

#include <cstdlib>
#include <iostream>
#include <curses.h>
#include <boost/noncopyable.hpp>


class ConsoleWindow: boost::noncopyable {
  public:
	ConsoleWindow() {
		if ((window = initscr()) == NULL) {
			std::cerr << "-!- Error initializing ncurses." << std::endl;
			exit(1);
		}
		oldcur = curs_set(0);

		if (COLS < 80 || LINES < 24) {
			cleanup();
			std::cerr << "-!- Error with console size:" << std::endl;
			std::cerr << "    COLS: " << COLS << ", minimum 80" << std::endl;
			std::cerr << "    LINES: " << LINES << ", minimum 24" << std::endl;
			exit(2);

		}

		cbreak(); // No line buffering
		noecho(); // Turn off key echoing
		keypad(window, true); // Enable the keypad for non-char keys

		start_color(); // Initialize colours
		if (has_colors() && COLOR_PAIRS >= 16 && COLORS >= 8) {
			use_default_colors();
			for (int i = 1; i < 8; i++) init_pair(i, i, -1);
		} else {
			cleanup();
			std::cerr << "-!- Error initializing colors:" << std::endl;
			std::cerr << "    has_colors: " << has_colors() << std::endl;
			std::cerr << "    COLOR_PAIRS: " << COLOR_PAIRS << std::endl;
			std::cerr << "    COLORS: " << COLORS << std::endl;
			exit(3);
		}
	}

	~ConsoleWindow() {
		cleanup();
	}

	void cleanup() {
		delwin(window);
		curs_set(oldcur);
		endwin();
	}

  private:
	WINDOW* window;
	int oldcur;
};
