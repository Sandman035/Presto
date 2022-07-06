#pragma once

#include <xcb/xcb.h>

namespace presto {
	struct WindowManager {
		xcb_connection_t* connection;
		xcb_screen_t* screen;

		bool shouldClose() { return _close; }

		friend void close(WindowManager* wm);

		private: bool _close = false;
	};

	int connect(WindowManager* wm);
	void run(WindowManager* wm);
	void close(WindowManager* wm);
	void disconnect(WindowManager* wm);
}
