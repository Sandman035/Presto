#include "window_manager.h"
#include "event.h"
#include "log.h"

#include <iostream>
#include <xcb/xcb_keysyms.h>
//#include <X11/keysymdef.h>

namespace presto {
	int connect(WindowManager* wm) {
		wm->connection = xcb_connect(NULL, NULL);

		if (xcb_connection_has_error(wm->connection)) {
			log::log("a connection error occured");
			return 1;
		}

		wm->screen = xcb_setup_roots_iterator(xcb_get_setup(wm->connection)).data;

		xcb_change_window_attributes(wm->connection, wm->screen->root, XCB_CW_EVENT_MASK, (uint32_t[]){XCB_EVENT_MASK_PROPERTY_CHANGE});
		xcb_ungrab_key(wm->connection, XCB_GRAB_ANY, wm->screen->root, XCB_MOD_MASK_ANY);

		//TODO: get and ungrab keybindings from config file

		xcb_flush(wm->connection);

		xcb_grab_button(wm->connection, 0, wm->screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE, 1, XCB_MOD_MASK_4);
		xcb_grab_button(wm->connection, 0, wm->screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE, 3, XCB_MOD_MASK_4);

		xcb_flush(wm->connection);

		return 0;
	}

	void run(WindowManager* wm) {
		if (xcb_connection_has_error(wm->connection)) {
			log::log("an error occured");
			close(wm);
			return;
		}

		xcb_generic_event_t * event = xcb_wait_for_event(wm->connection);
		if (event != NULL) {
			handleEvent(wm, event);

			free(event);
		}
		xcb_flush(wm->connection);
	}

	void close(WindowManager* wm) {
		wm->_close = true;
	}

	void disconnect(WindowManager* wm) {
		xcb_ungrab_key(wm->connection, XCB_GRAB_ANY, wm->screen->root, XCB_MOD_MASK_ANY);

		xcb_disconnect(wm->connection);
		log::log("disconnect from the x server");
	}
}
