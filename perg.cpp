#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unistd.h>
#include <regex>
#include <dirent.h>
#include <cstdlib>
#include <string.h>
// #include <omp.h>

struct Settings {
	Settings(): recursive(), invert(), verbose(), isFile(), file(), term() {}
	bool recursive;
	bool invert;
	bool verbose;
	bool isFile;
	std::string file;
	std::string term;
};

// Checks if the user asks for help.
// Parameters: argv (char* []) holds the arguments from the user.
void helpCheck(char *argv[]) {
	if (argv[1] == std::string("-h") || argv[1] == std::string("--help") || argv[1] == std::string("-help")) {
		std::cout << "\nPERG - Parallel Expression Reference Grep by Ryan Chui (2017)\n" << std::endl;
		std::cout << "    perg is a custom multithreaded c++ implementation of grep to search multi gigabyte files," << std::endl;
		std::cout << "    datasets, and directories developed at the National Center for Supercomputing Applications.\n" << std::endl;
		std::cout << "    Usage:" << std::endl;
		std::cout << "    perg [-r|-v|-V|-f <file>] <search term>\n" << std::endl;
		std::cout << "    Modes:" << std::endl;
		std::cout << "    -r    Recursive Search.     Recursively searches through the directory and all sub directories for the " << std::endl;
		std::cout << "                                given <search term>. Will not do anything if the [-f <file>] flag is given.\n" << std::endl;
		std::cout << "    -v    Search Inversion.     Search for every line that does not include the <search term>.\n" << std::endl;
		std::cout << "    -V    Enable Verbose.       The file path to the file will be printed along with the search result.\n" << std::endl;
		std::cout << "    -f    Single File Search.   Signals perg to only search the <file> for the <search term>. If -f is not" << std::endl;
		std::cout << "                                used, perg will search the entire directory from where perg is called from.\n" << std::endl;
		exit(0);
	}
}

void getSettings(int argc, char *argv[], Settings *instance) {
	std::queue<std::string> settings;
	for (int i = 1; i < argc; i++) {
		settings.push(argv[i]);
	}
	
	while (!settings.empty()) {
		std::string arg = settings.front();
		if (arg == "-r") {
			(*instance).recursive = true;
		} else if (arg == "-v") {
			(*instance).invert = true;
		} else if (arg == "-V") {
			(*instance).verbose = true;
		} else if (arg == "-f") {
			(*instance).isFile = true;
			settings.pop();
			(*instance).file = settings.front();
		} else {
			(*instance).term = settings.front();
		}
		settings.pop();
	}

	if ((*instance).term == "") {
		std::cout << "Search term not given. \"perg -h\" for help." << std::endl;
		exit(0);
	}
}

void printSingle(std::queue<std::string> *filePaths, Settings *instance) {
	while (!(*filePaths).empty()) {
		std::ifstream file1((*filePaths).front());
		std::ifstream file2((*filePaths).front());
		std::string line;
		std::regex rgx((*instance).term);
		int count = 0;

		for (int i = 0; std::getline(file1, line); ++i) {
			count++;
		}

		#pragma omp parallel for schedule(static)
		for (int i = 0; i < count; ++i) {
			#pragma omp critical
			std::getline(file2, line);
			if ((*instance).verbose) {
				if (!std::regex_search(line.begin(), line.end(), rgx) && (*instance).invert) {
					std::cout << (*filePaths).front() + ": " + line + "\n";
				} else if (std::regex_search(line.begin(), line.end(), rgx) && !(*instance).invert) {
					std::cout << (*filePaths).front() + ": " + line + "\n";
				}
			} else {
				if (!std::regex_search(line.begin(), line.end(), rgx) && (*instance).invert) {
					std::cout << line + "\n";
				} else if (std::regex_search(line.begin(), line.end(), rgx) && !(*instance).invert) {
					std::cout << line + "\n";
				}
			}
		}
		(*filePaths).pop();
	}
}

void findAll(std::queue<std::string> *filePaths, const char *cwd, Settings *instance) {
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(cwd)) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			std::string fileBuff = std::string(ent -> d_name);
			if (fileBuff != "." && fileBuff != "..") {
				DIR *dir2;
				std::string fileName = std::string(cwd) + "/" + fileBuff;
				if ((dir2 = opendir(fileName.c_str())) != NULL) {
					closedir(dir2);
					if ((*instance).recursive) {
						findAll(filePaths, fileName.c_str(), instance);
					}
				} else {
					(*filePaths).push(fileName);
				}
			}
		}
		closedir(dir);
	}
}

char cwd [PATH_MAX];

int main(int argc, char *argv[]) {
	Settings *instance = new Settings;
	std::queue<std::string> *filePaths = new std::queue<std::string>;

	helpCheck(argv);
	getSettings(argc, argv, instance);
	getcwd(cwd, PATH_MAX);
	if ((*instance).isFile) {
		(*filePaths).push(std::string(cwd) + "/" + (*instance).file);
		printSingle(filePaths, instance);
	} else {
		findAll(filePaths, cwd, instance);
		printSingle(filePaths, instance);
	}

	delete(filePaths);
	delete(instance);
	return 0;
}
