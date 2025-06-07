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

bool exit_flag = 0;
int yMax, xMax;
constexpr char OPERATIONS_COUNT = 5;
int selected = 0;

std::vector<fs::path> list_of_files() {
    std::vector<fs::path> files;
    fs::path current = fs::current_path();
    for (const auto& dir_entry : fs::directory_iterator(current)) {
        files.push_back(dir_entry.path());
    }
    return files;
}

std::vector<fs::path> list = list_of_files();

std::string operations[6] = {"1. Open", "2. Rename", "3. Delete", "4. Copy", "5. Move"};

void draw_menu(WINDOW* win, const std::vector<fs::path>& list, int selected) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "File Manager");
    mvwprintw(win, yMax - 2, xMax - 19, "Press ESC to exit");
    for(int i = 0; i < list.size(); i++) {
        if (i == selected) {
            wattron(win, A_REVERSE);
        }
        mvwprintw(win, i + 1, 1, list[i].filename().string().c_str());
        if (i == selected) {
            wattroff(win, A_REVERSE);
        }
    }
    mvwprintw(win, yMax - 2, 1, fs::current_path().string().c_str());
    wrefresh(win);
}

void draw_options(WINDOW* win, const std::string ops[], int size, int selected) {
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "Operations");
    for(int i = 0; i < size; i++) {
        if (i == selected) wattron(win, A_REVERSE);
        mvwprintw(win, i + 1, 1, ops[i].c_str());
        if (i == selected) wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

void update_file_name(WINDOW* win, const fs::path& file) {
	int max_x, max_y;
	
    curs_set(1);
	
	std::string current_filename = file.filename().string();
	std::string new_filename;
	getmaxyx(win, max_y, max_x);
	werase(win);
	box(win, 0, 0);
	std::string msg = "Selected: " + file.filename().string();
    mvwprintw(win, yMax - 2, 1, msg.c_str());
	wattron(win, A_BOLD | A_UNDERLINE);
	mvwprintw(win, 0, max_x/2 - 9, "Write new filename");
	wattroff(win, A_BOLD | A_UNDERLINE);
	int cur_x = 1;
	wmove(win, 1, 1);
	
	int ch = 0;
	
	while(true) {
		ch = wgetch(win);
		if (ch == 27) {break;}
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
                    cur_x--;
                }
            }
        } else if (ch >= 32 && ch < 256) {
            if (cur_x < max_x - 2) {
                new_filename.push_back(static_cast<char>(ch));
                waddch(win, ch);
                cur_x++;
            }
        }
        wrefresh(win);		
	}
	
	curs_set(0);
	if (new_filename.empty()) {
		return;
	} else {
		fs::rename(file, file.parent_path() / new_filename);
	}
	
	list = list_of_files();
	selected = 0;
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

void print_absolute_path(WINDOW* optionwin, const fs::path& file) {
    std::string msg = "Selected: " + file.filename().string();
    mvwprintw(optionwin, yMax - 2, 1, msg.c_str());
    wrefresh(optionwin);
}

void input_operation(WINDOW* optionwin, fs::path file) {
    int input = 0;
    
    int operation_selected = 0;
    
    draw_options(optionwin, operations, OPERATIONS_COUNT, operation_selected);
    print_absolute_path(optionwin, file);

    while(true) {
        input = wgetch(optionwin);
        
        switch(input) {
            case KEY_UP:
                operation_selected--;
                if (operation_selected < 0) operation_selected = 0;
                break;
            case KEY_DOWN:
                operation_selected++;
                if (operation_selected >= 5) operation_selected = 4;
                break;
            case 27: // ESC
                operation_selected = -1;
                werase(optionwin);
                draw_options(optionwin, operations, OPERATIONS_COUNT, operation_selected);
                return;
            case 10:
                if (operations[operation_selected] == "2. Rename") {
                	while(true) {
                    	update_file_name(optionwin, file);
                    	break;
                   	}
                }
                return;
            case 113:
            	exit_flag = 1;
                return;
            default:
                break;
        }
        
        draw_options(optionwin, operations, OPERATIONS_COUNT, operation_selected);
        print_absolute_path(optionwin, file);

        if (exit_flag) return;
    }
    
    draw_options(optionwin, operations, OPERATIONS_COUNT, operation_selected);
    return;
}

int main() {
	setlocale(LC_ALL, "");
	set_escdelay(25);
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    getmaxyx(stdscr, yMax, xMax);
    int divider = std::max(20, xMax / 2);
    
    WINDOW* menuwin = newwin(yMax, divider, 0, 0);
    WINDOW* optionwin = newwin(yMax, divider, 0, divider);
    keypad(menuwin, TRUE);
    
    int input;
    
    draw_menu(menuwin, list, selected);
    
    while(true) {
    	draw_file_info(optionwin, list[selected]);
    	
        input = wgetch(menuwin);
        
        switch(input) {
            case KEY_UP:
                selected--;
                if (selected < 0) selected = 0;
                break;
            case KEY_DOWN:
                selected++;
                if (selected >= list.size()) selected = list.size() - 1;
                break;
            case 10: // Enter
                {   
                    wmove(optionwin, 0, 1);
                    wrefresh(optionwin);
                    
                    keypad(optionwin, TRUE);
                    input_operation(optionwin, list[selected]);
                    keypad(optionwin, FALSE);
                    
					draw_menu(menuwin, list, selected); 
					wrefresh(menuwin);
                    break;
                }
            case 27: // ESC
                endwin();
                return 0;
            case 113: // 'q'
                exit_flag = 1;
                break;
            default:
                break;
        }

        if (exit_flag == 1) {
            break;
        }
		
        draw_menu(menuwin, list, selected);
    }
    
    delwin(menuwin);
    delwin(optionwin);

    endwin();
    return 0;
}
