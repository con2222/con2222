#include <iostream>
#include <filesystem>
#include <fstream>

using namespace std;
using namespace std::filesystem;

void print_directory_files(const path& current) {  // Добавьте параметр
    for (const auto& dir_entry : directory_iterator(current)) {
        cout << dir_entry.path().filename().string() << '\n';
    }
}

void enter_to_directory(path& current) {
	string directory_name;
	cin >> directory_name;
	if (directory_name == "..") {
		current = current.parent_path();
	} else {
		current /= directory_name;
	}
	current_path(current);
	//cout << current.string();
}

void copy_f(const path& current) {
	string source_name, dest_name;
	cin >> source_name >> dest_name;
	path source_path = current / source_name;
	path dest_path = current / dest_name;
	if (exists(dest_path) && is_directory(dest_path)) {
		dest_path /= source_path.filename();
	}
	copy_file(source_path, dest_path, copy_options::overwrite_existing);
	cout << dest_path << '\n';
}

void move(const path& current) {
	string source_name, dest_name;
	cin >> source_name >> dest_name;
	path source_path = current / source_name;
	path dest_path = current / dest_name;
	if (exists(dest_path) && is_directory(dest_path)) {
		dest_path /= source_path.filename();
	}
	copy_file(source_path, dest_path, copy_options::overwrite_existing);
	remove(source_path);
}

void create_file(const path& current) {
    string filename;
    cin >> filename;
    ofstream file(current / filename);  // Исправлено: current / filename
    if (!file) {
        cerr << "Ошибка создания файла: " << (current / filename) << endl;
    }
}

int main() {
	string option;
	cin >> option;
	path current = current_path();
	
	while(option != "exit") {
		if (option == "cd") {
			enter_to_directory(current);
		}
		if (option == "ls") {
			print_directory_files(current);
		}
		if (option == "cp") {
			copy_f(current);
		}
		if (option == "mv") {
			move(current);
		}
		if (option == "touch") {
			create_file(current);
		}
		if (option == "cat") {
		
		}
		cin >> option;
	}
    return 0;
}
