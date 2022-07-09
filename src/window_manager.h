#pragma once

#include <xcb/xcb.h>

namespace presto {
	struct WindowManager {
		xcb_connection_t* connection;
		xcb_screen_t* screen;
		xcb_drawable_t window;

		uint32_t value;

		uint32_t offsetX;
		uint32_t offsetY;

		bool shouldClose() { return _close; }

		friend void close(WindowManager* wm);

		private: bool _close = false;
	};

	int connect(WindowManager* wm);
	void run(WindowManager* wm);
	void close(WindowManager* wm);
	void disconnect(WindowManager* wm);
}
