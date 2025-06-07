#include <ncurses.h>
#include <cstdlib>

int main() {
    // Инициализация ncurses
    initscr();
    raw();
    //cbreak();           // Отключаем буферизацию строк
    noecho();           // Не отображаем вводимые символы
    keypad(stdscr, TRUE); // Включаем обработку спец. клавиш

    // Получаем размеры экрана
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // Размеры внутреннего окна
    int height = 10;
    int width = 40;
    int start_y = (rows - height) / 2;
    int start_x = (cols - width) / 2;

    // Создаем окно
    WINDOW* win = newwin(height, width, start_y, start_x);
    box(win, 0, 0); // Рисуем рамку
    mvwprintw(win, 1, 2, "Press any key (q to quit)");
    wrefresh(win);

    int ch;
    while ((ch = wgetch(win)) != 'q') {
        // Очищаем предыдущий вывод
        for (int i = 3; i < height - 2; ++i)
            mvwprintw(win, i, 2, "%*s", width - 4, ""); // очистка строки

        // Печатаем код символа
        mvwprintw(win, 3, 2, "Key code: %d", ch);
        wrefresh(win);
    }

    // Завершение работы
    delwin(win);
    endwin();
    return 0;
}
