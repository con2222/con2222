#include <iostream>
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>

namespace fs = std::filesystem;

std::vector<fs::path> list_of_files() {
    std::vector<fs::path> files;
    fs::path current = fs::current_path();
    for (const auto& dir_entry : fs::directory_iterator(current)) {
        files.push_back(dir_entry.path());
    }
    return files;
}

std::string operations[6] = {"1. Open", "2. Rename", "3. Delete", "4. Copy", "5. Move", "6. File info"};

void draw_menu(WINDOW* win, const std::vector<fs::path>& list, int highlight) {
    werase(win);
    int yMax, xMax;
    getmaxyx(win, yMax, xMax);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "File Manager");
    mvwprintw(win, yMax - 2, xMax - 19, "Press ESC to exit");
    if (highlight > 0 && highlight < (list.size() - 1)) {
    	mvchgat(selected+1, 1, maxOptionLength, A_STANDOUT, 1, NULL);	
    }	
    wrefresh(win);
}

void draw_options(WINDOW* win, const std::string ops[], int size) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "Operations");
    for(int i = 0; i < size; i++) {
        mvwprintw(win, i + 1, 1, ops[i].c_str());
    }
    wrefresh(win);
}

std::string get_time(const fs::path& file) {
    auto ftime = fs::last_write_time(file);
    
    auto duration = ftime.time_since_epoch();
    auto system_now = std::chrono::system_clock::now().time_since_epoch();
    auto file_now = decltype(ftime)::clock::now().time_since_epoch();
    auto system_duration = duration + (system_now - file_now);
    std::chrono::system_clock::time_point system_time(system_duration);
    time_t cftime = std::chrono::system_clock::to_time_t(system_time);
    
    char timeBuf[26];
    ctime_r(&cftime, timeBuf);
    timeBuf[24] = '\0';
    return std::string(timeBuf);
}

std::string get_permissions(const fs::path& file) {
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

void draw_file_info(WINDOW* win, fs::path file) {
    werase(win);
    int yMax;
    yMax = getmaxy(win);
    
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "File info");
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

void input_operation(WINDOW* optionwin, fs::path file) {
    int choice;
    int flag = 0;
    
    int operation_highlight = 0;
    
    for(int i = 0; i < 5; i++) {
        if (i == operation_highlight) wattron(optionwin, A_REVERSE);
        mvwprintw(optionwin, i + 1, 1, operations[i].c_str());
        if (i == operation_highlight) wattroff(optionwin, A_REVERSE);
    }
    wrefresh(optionwin);
    
    while(true) {
        choice = wgetch(optionwin);
        
        switch(choice) {
            case KEY_UP:
                operation_highlight--;
                if (operation_highlight < 0) operation_highlight = 0;
                break;
            case KEY_DOWN:
                operation_highlight++;
                if (operation_highlight >= 6) operation_highlight = 5;
                break;
            case 27: // ESC
                flag = 1;
                operation_highlight = -1;
                werase(optionwin);
                draw_options(optionwin, operations, 6);
                break;
            case 10:
                if (operations[operation_highlight] == "6. File info") {
                    while(true) {
                        draw_file_info(optionwin, file);
                        choice = wgetch(optionwin);
                        if (choice == 27) {
                            draw_options(optionwin, operations, 6);
                            break;
                        }
                    }
                }
            default:
                break;
        }
        
        for(int i = 0; i < 6; i++) {
            if (i == operation_highlight) wattron(optionwin, A_REVERSE);
            mvwprintw(optionwin, i + 1, 1, operations[i].c_str());
            if (i == operation_highlight) wattroff(optionwin, A_REVERSE);
        }
        wrefresh(optionwin);
        
        if (flag == 1) {
            break;
        }
    }
    
    draw_options(optionwin, operations, 6);
    return;
}

int main() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    int divider = xMax / 2;
    std::vector<fs::path> list = list_of_files();
    
    WINDOW* menuwin = newwin(yMax, divider, 0, 0);
    WINDOW* optionwin = newwin(yMax, divider, 0, divider);
    keypad(menuwin, TRUE);
    
    int highlight = 0;
    int choice = 0;
    
    draw_menu(menuwin, list, highlight);
    draw_options(optionwin, operations, 6);
    
    while(true) {
        draw_menu(menuwin, list, highlight);
        
        choice = wgetch(menuwin);
        
        switch(choice) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= list.size()) highlight = list.size() - 1;
                break;
            case 10: // Enter
                {
                    std::string msg = "Selected: " + list[highlight].filename().string();
                    mvwprintw(optionwin, 7, 1, msg.c_str());
                    wrefresh(optionwin);
                    
                    wmove(optionwin, 0, 1);
                    wrefresh(optionwin);
                    
                    keypad(optionwin, TRUE);
                    input_operation(optionwin, list[highlight]);
                    keypad(optionwin, FALSE);
                    
                    touchwin(menuwin);
                    wrefresh(menuwin);
                    break;
                }
            case 27: // ESC
                endwin();
                return 0;
            default:
                break;
        }
    }
    
    delwin(menuwin);
    delwin(optionwin);
    endwin();
    return 0;
}
