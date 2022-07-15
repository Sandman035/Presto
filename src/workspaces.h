#pragma once

#include <list>
#include <xcb/xcb.h>

namespace presto {
	struct WindowManager;

	struct Workspace {
		bool active = false;
		int monitor;

		std::list<xcb_window_t> windows;
	};

	void changeWorkspace(WindowManager* wm, int number);
}
