#pragma once

#include <cstdlib>
#include <boost/noncopyable.hpp>


class ConsoleWindow: boost::noncopyable {
  public:
	ConsoleWindow() {
		if ((window = initscr()) == NULL) {
			std::cerr << "-!- Error initializing ncurses." << std::endl;
			exit(1);
		}
		noecho(); // Turn off key echoing
		keypad(window, true); // Enable the keypad for non-char keys
		oldcur = curs_set(0);

		start_color(); // Initialize colours
		if (has_colors() && COLOR_PAIRS >= 16 && COLORS >= 8) {
			use_default_colors();
			for (int i = 1; i < 4; i++) init_pair(i, i, -1);
		} else {
			cleanup();
			std::cerr << "-!- Error initializing colors:" << std::endl;
			std::cerr << "    has_colors: " << has_colors() << std::endl;
			std::cerr << "    COLOR_PAIRS: " << COLOR_PAIRS << std::endl;
			std::cerr << "    COLORS: " << COLORS << std::endl;
			exit(2);
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
