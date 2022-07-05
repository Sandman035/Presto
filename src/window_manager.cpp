#include "window_manager.h"

#include <iostream>
#include <xcb/xcb_keysyms.h>

namespace presto {
	int connect(WindowManager* wm) {
		wm->connection = xcb_connect(NULL, NULL);

		if (xcb_connection_has_error(wm->connection)) {
			std::cout << "a connection error occured\n";
			return 1;
		}

		wm->screen = xcb_setup_roots_iterator(xcb_get_setup(wm->connection)).data;

		xcb_change_window_attributes(wm->connection, wm->screen->root, XCB_CW_EVENT_MASK, (uint32_t[]){XCB_EVENT_MASK_PROPERTY_CHANGE});
		xcb_ungrab_key(wm->connection, XCB_GRAB_ANY, wm->screen->root, XCB_MOD_MASK_ANY);

		xcb_flush(wm->connection);

		xcb_grab_button(wm->connection, 0, wm->screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE, 1, XCB_MOD_MASK_4);
		xcb_grab_button(wm->connection, 0, wm->screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE, 3, XCB_MOD_MASK_4);

		xcb_flush(wm->connection);

		return 0;
	}

	void close(WindowManager* wm) {
		wm->_close = true;
	}

	void disconnect(WindowManager* wm) {
		xcb_disconnect(wm->connection);
		std::cout << "disconnected from the x server\n";
	}
}
