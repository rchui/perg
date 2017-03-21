#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unistd.h>
#include <regex>
#include <dirent.h>
#include <cstdlib>
#include <string.h>
#include <sstream>
#include <omp.h>

// Holds the user-given settings that modify perg behavior.
struct Settings {
	Settings(): recursive(),
				invert(),
				verbose(),
				isFile(),
				fileWise(),
				checkHidden(),
				file(),
				term() {}
	bool recursive;
	bool invert;
	bool verbose;
	bool isFile;
	bool fileWise;
	bool checkHidden;
	std::string file;
	std::string term;
};

// Checks if the user asks for help.
// Parameters: argv (char* []) holds the arguments from the user.
void helpCheck(char *argv[]) {
	if (argv[1] == std::string("-h") || argv[1] == std::string("--help") || argv[1] == std::string("-help")) {
		std::cout << "\nPERG - Parallel Exploritory Recursive Grep by Ryan Chui (2017)\n" << std::endl;
		std::cout << "    perg is a custom multithreaded c++ implementation of grep to search multi gigabyte files, datasets, and\n";
		std::cout << "    directories developed at the National Center for Supercomputing Applications.\n" << std::endl;
		std::cout << "    Usage:\n";
		std::cout << "    perg [-f <file>|-r|-v|-V|-w] <search term>\n" << std::endl;
		std::cout << "    Modes:\n";
		std::cout << "    -f    Single File Search    Signals perg to only search the <file> for the <search term>. If -f is not\n";
		std::cout << "                                used, perg will search the entire directory from where perg is called from.\n" << std::endl;
		std::cout << "    -i    Include Hidden        Will include hidden files in the search. Default search behavior is to\n";
		std::cout << "                                ignore hidden files." << std::endl;
		std::cout << "    -r    Recursive Search      Recursively searches through the directory and all sub directories for the \n";
		std::cout << "                                given <search term>. Will not do anything if the [-f <file>] flag is given.\n" << std::endl;
		std::cout << "    -v    Search Inversion      Search for every line that does not include the <search term>.\n" << std::endl;
		std::cout << "    -V    Enable Verbose        The file path to the file will be printed along with the search result.\n" << std::endl;
		std::cout << "    -w    File Parallelism      Signals perg to perform single-threaded searches of multiple files. Default\n";
		std::cout << "                                search behavior is to search files one at a time with mulitple threads.\n";
		std::cout << "                                This is optimal when the files are small, similar size, or there are many.\n" << std::endl;
		exit(0);
	}
}

// Gets the settings given by the user.
// Parameters: argc (int) number of user arguments.
//             argv (char *[]) user arguments
//             instance (Settings *) user argument container.
void getSettings(int argc, char *argv[], Settings *instance) {
	std::queue<std::string> settings;
	for (int i = 1; i < argc; i++) {
		settings.push(argv[i]);
	}
	
	// Sets settings based off of arguments received.
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
			if (arg.compare(0, 1, "-") == 0) {
				std::cout << "ERROR: The path to the file was not given. \"perg -h\" for help." << std::endl;
				exit(0);
			}
			(*instance).file = settings.front();
		} else if (arg == "-w") {
			(*instance).fileWise = true;
		} else if (arg == "-i") {
			(*instance).checkHidden = true;
		} else {
			if (settings.size() > 1) {
				std::cout << "ERROR: perg was called incorrectly. \"perg -h\" for command syntax." << std::endl;
				exit(0);
			}
			(*instance).term = settings.front();
		}
		settings.pop();
	}

	// Check that the search term has been given.
	if ((*instance).term == "") {
		std::cout << "ERROR: Search term not given. \"perg -h\" for help." << std::endl;
		exit(0);
	}
}

// Searches and prints results in a file-wise manner.
// Parameters: filePaths (std::queue<std::string> *) filepaths of files to search.
//             instance (Settings *) user argument container.
void printMultiple(std::queue<std::string> *filePaths, Settings *instance) {
	std::regex rgx((*instance).term);
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < (int) (*filePaths).size(); ++i) {
		std::string fileName;

		#pragma omp critical
		{
			fileName = (*filePaths).front();
			(*filePaths).pop();
		}

		std::ifstream file(fileName);
		std::string line;

		// Check each line and print results.
		while (std::getline(file, line)) {
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
	}
}

// Searches and prints results in line-wise.
// Parameters: filePaths (std::queue<std::string> *) filepaths of files to search.
//             instance (Settings *) user argument container.
void printSingle(std::queue<std::string> *filePaths, Settings *instance) {
	while (!(*filePaths).empty()) {
		std::ifstream file1((*filePaths).front());
		std::ifstream file2((*filePaths).front());
		std::string line1;
		std::regex rgx((*instance).term);
		int count = 0;

		for (int i = 0; std::getline(file1, line1); ++i) {
			count++;
		}

		// Check each line and print results.
		#pragma omp parallel for schedule(static)
		for (int i = 0; i < count; ++i) {
			std::string line2;
			#pragma omp critical
			std::getline(file2, line2);
			if ((*instance).verbose) {
				if (!std::regex_search(line2.begin(), line2.end(), rgx) && (*instance).invert) {
					std::cout << (*filePaths).front() + ": " + line2 + "\n";
				} else if (std::regex_search(line2.begin(), line2.end(), rgx) && !(*instance).invert) {
					std::cout << (*filePaths).front() + ": " + line2 + "\n";
				}
			} else {
				if (!std::regex_search(line2.begin(), line2.end(), rgx) && (*instance).invert) {
					std::cout << line2 + "\n";
				} else if (std::regex_search(line2.begin(), line2.end(), rgx) && !(*instance).invert) {
					std::cout << line2 + "\n";
				}
			}
		}
		(*filePaths).pop();
	}
}

// Recursively find and stores all file paths to search.
// Parameters: filePaths (std::queue<std::string> *) filepaths of files to search.
//             cwd (const char *) current working directory of files to store.
//             instance (Settings *) user argument container.
void findAll(std::queue<std::string> *filePaths, const char *cwd, Settings *instance) {
	DIR *dir;
	struct dirent *ent;

	// Check if cwd is a directory.
	if ((dir = opendir(cwd)) != NULL) {
		// Get all file paths within directory.
		while ((ent = readdir (dir)) != NULL) {
			std::string fileBuff = std::string(ent -> d_name);
			if (fileBuff != "." && fileBuff != "..") {
				if ((*instance).checkHidden) {	
					DIR *dir2;
					std::string fileName = std::string(cwd) + "/" + fileBuff;
					// Check if file path is a directory.
					if ((dir2 = opendir(fileName.c_str())) != NULL) {
						closedir(dir2);
						if ((*instance).recursive) {
							findAll(filePaths, fileName.c_str(), instance);
						}
					} else {
						(*filePaths).push(fileName);
					}
				} else {
					if (fileBuff[0] != '.') {	
						DIR *dir2;
						std::string fileName = std::string(cwd) + "/" + fileBuff;
						// Check if file path is a directory.
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
			}
		}
		closedir(dir);
	}
}

char cwd [PATH_MAX];

// Checks to see if the user asks for help.
// Gets the settings set by the user.
// Stores the file names of all files to be searched.
// Prints search results.
// Parameters: argc (int) number of user arguments.
//             argv (char *[]) user given arguments.
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
		if ((*instance).fileWise) {
			printMultiple(filePaths, instance);
		} else {
			printSingle(filePaths, instance);
		}
	}

	delete(filePaths);
	delete(instance);
	return 0;
}
