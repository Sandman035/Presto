#pragma once

#include <xcb/xcb.h>
#include <vector>

#include "monitors.h"
#include "workspaces.h"

namespace presto {
	struct KeyBindings {
		xcb_keysym_t keyCode;
		uint16_t mod;
	};

	struct WindowManager {
		xcb_connection_t* connection;
		xcb_screen_t* screen;

		std::vector<Monitor> monitors;
		Workspace workspaces[10];

		xcb_window_t window;

		int keyWorkspaces[4] = {
			1,
			2,
			3,
			4
		};

		uint32_t value;

		uint32_t offsetX;
		uint32_t offsetY;

		uint32_t offsetX2;
		uint32_t offsetY2;

		bool shouldClose() { return _close; }

		friend void close(WindowManager* wm);

		private: bool _close = false;
	};

	int connect(WindowManager* wm);
	void run(WindowManager* wm);
	void close(WindowManager* wm);
	void disconnect(WindowManager* wm);
}
