#pragma once

#include <vector>
#include <boost/noncopyable.hpp>

#include "rlutil.h"
#include "common.hh"

class ConsoleWindow: boost::noncopyable {
  public:
	ConsoleWindow(int w, int h): windowW(w), windowH(h), curX(), curY() {
		for (int j = 0; j < h; j++) {
			std::vector<Tile> row;
			for (int i = 0; i < w; i++) row.push_back(Tile());
			cells.push_back(row);
		}
		rlutil::hidecursor();
	}
	~ConsoleWindow() {
		rlutil::showcursor();
	}

	/// Function: print
	void print(std::string txt, int x, int y, int color) {
		if (x < 0 || x >=windowW || y < 0 || y >= windowH) return;
		//for (int i = 0; i < ((x+int(txt.length()) >= windowW) ? windowW-x : int(txt.length())); i++) {
			for (int i = 0; i < int(txt.length()); i++) {
			cells[y][x+i].ch = txt[i];
			cells[y][x+i].color = color;
			cells[y][x+i].changed = true;
		}
	}

	/// Function: put
	void put(char ch, int x, int y, int color) {
		if (x < 0 || x >=windowW || y < 0 || y >= windowH) return;
		if (cells[y][x].ch != ch || cells[y][x].color != color) {
			cells[y][x].ch = ch;
			cells[y][x].color = color;
			cells[y][x].changed = true;
		}
	}

	/// Function refresh
	void refresh() {
		for (int j = 0; j < windowH; j++) {
			for (int i = 0; i < windowW; i++) {
				if (cells[j][i].changed) {
					rlutil::locate(i, j);
					rlutil::setColor(cells[j][i].color);
					std::cout << cells[j][i].ch;
					cells[j][i].changed = false;
				}
			}
		}
		curX = 0; curY = 0;
	}

	/// Function redraw
	void redraw() {
		int prevC = -1, curC = -1;
		for (int j = 0; j < windowH; j++) {
			for (int i = 0; i < windowW; i++) {
				if ((curC = cells[j][i].color) != prevC) {
					rlutil::setColor(curC);
					prevC = curC;
				}
				std::cout << cells[j][i].ch;
				cells[j][i].changed = false;
			}
			std::cout << std::endl;
		}
		curX = 0; curY = 0;
	}

  private:
	int windowW;
	int windowH;
	int curX;
	int curY;
	tilearray cells;
};
