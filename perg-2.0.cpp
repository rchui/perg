#include <iostream>
#include <string>
#include <queue>

struct Settings {
	bool recursive;
	bool invert;
	bool verbose;
	bool isFile;
	std::string file;
	int terms;
};

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
			(*instance).invert = true;
		} else if (arg == "-f") {
			(*instance).isFile = true;
			settings.pop();
			(*instance).file = settings.front();
		}
		settings.pop();
	}
}

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

int main(int argc, char *argv[]) {
	Settings *instance = new Settings;
	helpCheck(argv);
	getSettings(argc, argv, instance);
	return 0;
}
