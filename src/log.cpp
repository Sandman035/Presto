#include "log.h"

#include <iostream>
#include <fstream>

namespace log {
	std::ofstream logFile;

	void init() {
		logFile.open("presto_log.txt");
	}

	void log(std::string message) {
		std::cout << message << "\n";
		logFile << message << "\n";
	}

	void shutdown() {
		logFile.close();
	}
}
