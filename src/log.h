#pragma once

#include <string>

namespace log {
	void init();
	void log(std::string message);
	void shutdown();
}
