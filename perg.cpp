#include <iostream>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <typeinfo>
#include <stdlib.h>
#include <string>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <omp.h>
#include <string.h>

// Holds the options given by the user.
struct Settings {
	bool recursive;
	bool invert;
	bool verbose;
	bool isFile;
	std::string file;
	int terms;
};

// Checks if the user asks for help.
// Parameters: argv (char* []) holds the arguments from the user.
void helpCheck(char *argv[]) {
	if (argv[1] == std::string("-h") || argv[1] == std::string("--help") || argv[1] == std::string("-help")) {
		std::cout << "\nPERG - Parallel Expression Reference Grep by Ryan Chui (2017)\n" << std::endl;
		std::cout << "    perg is a custom multithreaded c++ implementation of grep to search multi gigabyte files," << std::endl;
		std::cout << "    datasets, and directories developed at the National Center for Supercomputing Applications.\n" << std::endl;
		std::cout << "    Due to the nature of implementation the order of syntax is strict and must be adhered to or " << std::endl;
		std::cout << "    errors will result. The order flags must always be used in is [-r|-v|-V|-f].\n" << std::endl;
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

// Checks if recurrsive and if the output is inverted.
// Parameters: argc (int) holds the number of arguments.
//             argv (char* []) holds the arguments from the user.
Settings getSettings(int argc, char *argv[]) {
	int term = 1;
	Settings instance;
	if (argv[term] == std::string("-r")) {
		term++;
		instance.recursive = true;
	} else {
		instance.recursive = false;
	}
	if (argv[term] == std::string("-v")) {
		term++;
		instance.invert = true;
	} else {
		instance.invert = false;
	}
	if (argv[term] == std::string("-V")) {
		term++;
		instance.verbose = true;
	} else {
		instance.verbose = false;
	}
	if (argv[term] == std::string("-f")) {
		instance.isFile = true;
		instance.file = argv[term + 1];
		term += 2;
	} else {
		instance.isFile = false;
		instance.file = "";
	}
	instance.terms = term;
	return instance;
}

// Gets the term to be sercahed for.
// Parameters: instance (Setting) holds the user options.
//             argv (char* []) holds the options given by the user.
std::string getSearchTerm(Settings instance, char *argv[]) {
	return std::string(argv[instance.terms]);
}


// Get and print the names of all the files
// Parameters: cwd (char*) holds the current working directory.
//             names (std::vector<std::string>) holds the names of all files and directories.
//             term (std::string) holds the term to search for.
//             base (int) holds the recurrsion level.
//             instance (Settings) holds the user options.
//             count (int*) holds the number of files and directories found.
std::vector<std::string> findAll(char *cwd, std::vector<std::string> names, std::string term, int base, Settings instance, int *count) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(cwd)) != NULL) {
		while((ent = readdir (dir)) != NULL) {
			std::string fileBuff = std::string(ent->d_name);
			std::string fileName = std::string(cwd) + "/" + fileBuff;
			DIR *dir2;
			if (fileBuff != "." && fileBuff != "..") {	
				names[*count] = fileName;
				*count += 1;
			}
			if ((dir2 = opendir(strdup(fileName.c_str()))) != NULL && fileBuff != "." && fileBuff != ".." && instance.recursive == true) {
				names = findAll(strdup(fileName.c_str()), names, term, base + 1, instance, count);
			}
			if (*count == names.size()) {
				names.resize(*count * 2);
			}
		}
		closedir (dir);
	}
	if (base == 0) {
		#pragma omp parallel for schedule(dynamic)
		for (int i = 0; i < names.size(); ++i) {
			if (names[i] != "") {
				std::ifstream file(names[i].c_str());
				std::string line;
				while (std::getline(file, line)) {
					std::stringstream stream;
					if (instance.verbose == false) {
						if (line.find(term) != std::string::npos && instance.invert == false) {
							stream << strdup(line.c_str()) << std::endl;
						}
						if (!(line.find(term) != std::string::npos) && instance.invert == true) {
							stream << strdup(line.c_str()) << std::endl;
						}
					} else {
						if (line.find(term) != std::string::npos && instance.invert == false) {
							stream << strdup(names[i].c_str()) << ": " << strdup(line.c_str()) << std::endl;
						}
						if (!(line.find(term) != std::string::npos) && instance.invert == true) {
							stream << strdup(names[i].c_str()) << ": " << strdup(line.c_str()) << std::endl;
						}
					}
					std::cout << stream.str();
				}
			}
		}
	}
	return names;
}

// Search for term in a single file
// Parameters: instance (Settings) holds the user given options.
//             term (std::string) holds the term to search for.
//             cwd (char*) current working directory.
void printSingle(Settings instance, std::string term, char *cwd) {
	std::ifstream file1(instance.file.c_str());
	std::ifstream file2(instance.file.c_str());
	std::string line;
	std::string fileName = std::string(cwd) + "/" + instance.file;
	int count = 0;
	for (int i = 0; std::getline(file1, line); ++i) {
		count++;
	}
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < count; ++i) {
		#pragma omp critical
		std::getline(file2, line);
		if (instance.verbose == false) {
			if (line.find(term) != std::string::npos && instance.invert == false) {
				printf("%s\n", strdup(line.c_str()));
			}
			if (!(line.find(term) != std::string::npos) && instance.invert == true) {
				printf("%s\n", strdup(line.c_str()));
			}
		} else {
			if (line.find(term) != std::string::npos && instance.invert == false) {
				printf("%s%s%s\n", strdup(fileName.c_str()), ": ", strdup(line.c_str()));
			}
			if (!(line.find(term) != std::string::npos) && instance.invert == true) {
				printf("%s%s%s\n", strdup(fileName.c_str()), ": ", strdup(line.c_str()));
			}
		}
	}
}

// Checks if the user asks for help
// Gets the options given by the user.
// Gets the term searched for by the user.
// Searches for the term in a file or all files.
int main(int argc, char *argv[]) {
	char *cwd = (char*) malloc(sizeof(char) * PATH_MAX);
	helpCheck(argv);
	std::vector<std::string> names (21);
	Settings instance = getSettings(argc, argv);
	std::string term = getSearchTerm(instance, argv);
	cwd = get_current_dir_name();
	int base = 0;
	int *count = new int(1);
	if (instance.isFile == false) {
		findAll(cwd, names, term, base, instance, count);
	} else {
		printSingle(instance, term, cwd);
	}
	free(cwd);
	delete(count);
	return 0;
}
