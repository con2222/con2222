#include <ncurses.h>
#include <iostream>

using namespace std;


int main() {
	initscr();
	noecho();
	cbreak();
	int yMax, xMax;
	getmaxyx(stdscr, yMax, xMax);
// Размеры разделителя
	int divider = xMax / 2;

	// Окна
	WINDOW* menuwin = newwin(yMax, divider, 0, 0);
	WINDOW* optionwin = newwin(yMax, divider - 1, 0, divider + 1); // +1 для разделителя

	// Рамки
	box(menuwin, 0, 0);
	box(optionwin, 0, 0);

	// Разделитель
	//mvaddch(0, divider, ACS_TTEE);       // Верхний угол
	mvvline(0, divider, ACS_VLINE, yMax); // Линия
	//mvaddch(yMax - 1, divider, ACS_BTEE); // Нижний угол

	// Обновление
	wrefresh(menuwin);
	wrefresh(optionwin);
	
	getch();
	endwin();
}
