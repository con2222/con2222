#include <iostream>
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <vector>

using namespace std::filesystem;
using namespace std;

class DefaultWindow {
	WINDOW* win {};

	Window(int nlines, int ncols, int begin_y, int begin_x) {
		win = newwin(nlines, ncols, begin_y, begin_x);
		box(win, 0, 0);
	}
	
	~Window() {
		delwin(win);
	}
};




vector<path> list_of_files() {
	vector<path> files;
	path current = current_path();
	for (const auto& dir_entry : directory_iterator(current)) {
        //cout << dir_entry.path().filename().string() << '\n';
        files.push_back(dir_entry.path());
    }
    return files;
}

int main() {
	initscr();
	noecho();
	cbreak();
	
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);
	int divider = xMax / 2;
	vector<path> list = list_of_files();
	
	WINDOW* menuwin = newwin(yMax, divider, 0, 0);
	WINDOW* optionwin = newwin(yMax, divider, 0, divider);
	box(menuwin, 0, 0);
	box(optionwin, 0, 0);
	refresh();
	wrefresh(menuwin);
	wrefresh(optionwin);
	keypad(menuwin, true);
	
	int highlight = 0;
	int choice;
	int counter = 0;
	int flag = 0;
	
	while(true) {
		for(int i = 0; i < list.size(); i++) {
			if (i == highlight) {
				wattron(menuwin, A_REVERSE);
				flag = 1;
			}
			mvwprintw(menuwin, i + 1, 1, list[i].filename().string().c_str());
			if (flag == 1) {wattroff(menuwin, A_REVERSE);}
		}
		choice = wgetch(menuwin);
		counter++;
		switch(choice) {
			case KEY_UP:
				highlight--;
				if (highlight == -1) {
					highlight = 0;
				}
				break;
			case KEY_DOWN:
				highlight++;
				if (highlight == list.size()) {
					highlight = list.size() - 1;
				}
				break;
			default:
				break;
		}
		if (choice == 10) {
			clear();
			mvprintw(yMax/2, xMax/2, "%s", list[highlight].filename().string().c_str());
			break;
		}
	}
	
	getch();
	endwin();
	return 0;
}
