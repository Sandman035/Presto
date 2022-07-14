#pragma once

#include <list>
#include <xcb/xcb.h>

namespace presto {
	struct Workspace {
		bool active = false;
		int monitor;

		std::list<xcb_window_t> windows;
	};
}
