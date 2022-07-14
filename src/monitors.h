#pragma once

#include <cstdint>
#include <xcb/xcb.h>
#include <vector>

namespace presto {
	struct Monitor {
		int16_t x;
		int16_t y;
		uint16_t width;
		uint16_t height;

		int currentWorkspace;
	};

	std::vector<Monitor> getMonitors(xcb_connection_t* connection, xcb_window_t window);

	int getMonitorUnderCursor(xcb_connection_t* connection, xcb_window_t root, std::vector<Monitor> monitors);
}
