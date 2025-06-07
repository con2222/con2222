#include <iostream>
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <locale.h>

namespace fs = std::filesystem;

class FileManager {
private:
    bool exit_flag = false;
    int yMax, xMax;
    int selected = 0;
    std::vector<fs::path> list;
    const std::vector<std::string> operations = {"1. Open", "2. Rename", "3. Delete", "4. Copy", "5. Move"};

    std::vector<fs::path> update_file_list() {
        std::vector<fs::path> files;
        fs::path current = fs::current_path();
        for (const auto& dir_entry : fs::directory_iterator(current)) {
            files.push_back(dir_entry.path());
        }
        return files;
    }

public:
    FileManager() {
        list = update_file_list();
    }

    bool should_exit() const { return exit_flag; }

    void set_exit(bool flag) { exit_flag = flag; }

    void set_window_size(int y, int x) {
        yMax = y;
        xMax = x;
    }

    void draw_menu(WINDOW* win) const {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 1, "File Manager");
        mvwprintw(win, yMax - 2, xMax - 19, "Press ESC to exit");
        
        for(int i = 0; i < list.size(); i++) {
            if (i == selected) wattron(win, A_REVERSE);
            mvwprintw(win, i + 1, 1, list[i].filename().string().c_str());
            if (i == selected) wattroff(win, A_REVERSE);
        }
        
        mvwprintw(win, yMax - 2, 1, fs::current_path().string().c_str());
        wrefresh(win);
    }

    void draw_options(WINDOW* win, int operation_selected) const {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 1, "Operations");
        
        for(int i = 0; i < operations.size(); i++) {
            if (i == operation_selected) wattron(win, A_REVERSE);
            mvwprintw(win, i + 1, 1, operations[i].c_str());
            if (i == operation_selected) wattroff(win, A_REVERSE);
        }
        
        wrefresh(win);
    }

    void update_selected(int change) {
        selected += change;
        if (selected < 0) selected = 0;
        if (selected >= list.size()) selected = list.size() - 1;
    }

    void rename_file(WINDOW* win, const fs::path& file) {
        curs_set(1);
        std::string current_filename = file.filename().string();
        std::string new_filename;
        
        werase(win);
        box(win, 0, 0);
        std::string msg = "Selected: " + file.filename().string();
        mvwprintw(win, yMax - 2, 1, msg.c_str());
        
        wattron(win, A_BOLD | A_UNDERLINE);
        mvwprintw(win, 0, xMax/2 - 9, "Write new filename");
        wattroff(win, A_BOLD | A_UNDERLINE);
        
        wmove(win, 1, 1);
        int ch = 0;
        
        while(true) {
            ch = wgetch(win);
            if (ch == '\n' || ch == KEY_ENTER) {
                break;
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
                if (!new_filename.empty()) {
                    int y, x;
                    getyx(win, y, x);
                    if (x > 1) {
                        new_filename.pop_back();
                        wmove(win, y, x - 1);
                        waddch(win, ' ');
                        wmove(win, y, x - 1);
                    }
                }
            } else if (ch >= 32 && ch < 256) {
                if (new_filename.length() < static_cast<size_t>(xMax - 2)) {
                    new_filename.push_back(static_cast<char>(ch));
                    waddch(win, ch);
                }
            }
            wrefresh(win);
        }

        if (!new_filename.empty()) {
            fs::rename(file, file.parent_path() / new_filename);
            list = update_file_list();
            selected = 0;
        }
        
        curs_set(0);
    }

    static std::string get_time(const fs::path& file) {
        auto ftime = fs::last_write_time(file);
        auto system_now = std::chrono::system_clock::now().time_since_epoch();
        auto file_now = decltype(ftime)::clock::now().time_since_epoch();
        auto system_duration = ftime.time_since_epoch() + (system_now - file_now);
        
        time_t cftime = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::time_point(system_duration));
        
        char timeBuf[26];
        ctime_r(&cftime, timeBuf);
        timeBuf[24] = '\0';
        return std::string(timeBuf);
    }

    static std::string get_permissions(const fs::path& file) {
        auto perms = fs::status(file).permissions();
        std::string permissions;
        permissions += (perms & fs::perms::owner_read) != fs::perms::none ? "r" : "-";
        permissions += (perms & fs::perms::owner_write) != fs::perms::none ? "w" : "-";
        permissions += (perms & fs::perms::owner_exec) != fs::perms::none ? "x" : "-";
        permissions += (perms & fs::perms::group_read) != fs::perms::none ? "r" : "-";
        permissions += (perms & fs::perms::group_write) != fs::perms::none ? "w" : "-";
        permissions += (perms & fs::perms::group_exec) != fs::perms::none ? "x" : "-";
        permissions += (perms & fs::perms::others_read) != fs::perms::none ? "r" : "-";
        permissions += (perms & fs::perms::others_write) != fs::perms::none ? "w" : "-";
        permissions += (perms & fs::perms::others_exec) != fs::perms::none ? "x" : "-";
        return permissions;
    }

    void draw_file_info(WINDOW* win) const {
        if (list.empty()) return;
        
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 1, "File info");
        
        const auto& file = list[selected];
        std::string fileName = "Name: " + file.filename().string();
        std::string fileSize = "Size: " + std::to_string(fs::file_size(file));
        std::string fileExt = "Extension: " + std::string(file.extension());
        std::string permissions = "Permissions: " + get_permissions(file);
        std::string lastWriteTime = "Last Update Time: " + get_time(file);
        
        mvwprintw(win, 1, 1, fileName.c_str());
        mvwprintw(win, 2, 1, fileSize.c_str());
        mvwprintw(win, 3, 1, fileExt.c_str());
        mvwprintw(win, 4, 1, permissions.c_str());
        mvwprintw(win, 5, 1, lastWriteTime.c_str());
        
        mvwprintw(win, yMax - 2, 1, fs::absolute(file).c_str());
        wrefresh(win);
    }

    void print_selected_path(WINDOW* win) const {
        if (list.empty()) return;
        std::string msg = "Selected: " + list[selected].filename().string();
        mvwprintw(win, yMax - 2, 1, msg.c_str());
        wrefresh(win);
    }

    void handle_operation(WINDOW* optionwin) {
        if (list.empty()) return;
        
        int input = 0;
        int operation_selected = 0;
        
        draw_options(optionwin, operation_selected);
        print_selected_path(optionwin);

        while(true) {
            input = wgetch(optionwin);
            
            switch(input) {
                case KEY_UP:
                    operation_selected--;
                    if (operation_selected < 0) operation_selected = 0;
                    break;
                case KEY_DOWN:
                    operation_selected++;
                    if (operation_selected >= static_cast<int>(operations.size())) {
                        operation_selected = operations.size() - 1;
                    }
                    break;
                case 27: // ESC
                    werase(optionwin);
                    draw_options(optionwin, -1);
                    return;
                case 10: // Enter
                    if (operation_selected == 1) { // Rename
                        rename_file(optionwin, list[selected]);
                    }
                    return;
                case 'q':
                    exit_flag = true;
                    return;
                default:
                    break;
            }
            
            draw_options(optionwin, operation_selected);
            print_selected_path(optionwin);

            if (exit_flag) return;
        }
    }
};

int main() {
    setlocale(LC_ALL, "");
    set_escdelay(25);
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int divider = std::max(20, xMax / 2);
    
    WINDOW* menuwin = newwin(yMax, divider, 0, 0);
    WINDOW* optionwin = newwin(yMax, divider, 0, divider);
    keypad(menuwin, TRUE);
    
    FileManager fm;
    fm.set_window_size(yMax, divider);

    fm.draw_menu(menuwin);
    
    while(!fm.should_exit()) {
        fm.draw_file_info(optionwin);
        
        int input = wgetch(menuwin);
        
        switch(input) {
            case KEY_UP:
                fm.update_selected(-1);
                break;
            case KEY_DOWN:
                fm.update_selected(1);
                break;
            case 10: // Enter
                keypad(optionwin, TRUE);
                fm.handle_operation(optionwin);
                keypad(optionwin, FALSE);
                fm.draw_menu(menuwin);
                break;
            case 27: // ESC
                fm.set_exit(true);
                break;
            case 'q':
                fm.set_exit(true);
                break;
            default:
                break;
        }
        
        fm.draw_menu(menuwin);
    }
    
    delwin(menuwin);
    delwin(optionwin);
    endwin();
    return 0;
}
