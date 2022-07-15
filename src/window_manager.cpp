#include "window_manager.h"
#include "event.h"
#include "log.h"

#include <iostream>
#include <xcb/xcb_keysyms.h>
#include <X11/keysymdef.h>
#include <xcb/xcb_cursor.h>

namespace presto {
	KeyBindings keys[] {
		{0x0078, XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT},
		{0x0030, XCB_MOD_MASK_4},
		{0x0031, XCB_MOD_MASK_4},
		{0x0032, XCB_MOD_MASK_4},
		{0x0033, XCB_MOD_MASK_4},
		{0x0034, XCB_MOD_MASK_4},
		{0x0035, XCB_MOD_MASK_4},
		{0x0036, XCB_MOD_MASK_4},
		{0x0037, XCB_MOD_MASK_4},
		{0x0038, XCB_MOD_MASK_4},
		{0x0039, XCB_MOD_MASK_4},
		{0x0070, XCB_MOD_MASK_4},
		{0x0068, XCB_MOD_MASK_4},
		{0x006a, XCB_MOD_MASK_4},
		{0x006b, XCB_MOD_MASK_4},
		{0x006c, XCB_MOD_MASK_4},
		{0x0068, XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT},
		{0x006a, XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT},
		{0x006b, XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT},
		{0x006c, XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT},
	};

	int connect(WindowManager* wm) {
		wm->connection = xcb_connect(NULL, NULL);

		if (xcb_connection_has_error(wm->connection)) {
			log::log("a connection error occured");
			return 1;
		}

		wm->screen = xcb_setup_roots_iterator(xcb_get_setup(wm->connection)).data;

		wm->monitors = getMonitors(wm->connection, wm->screen->root);
		if (!wm->monitors.empty()) {
			for (int i = 0; i < wm->monitors.size(); i++) {
				wm->monitors[i].currentWorkspace = i + 1;
				wm->workspaces[i + 1].active = true;
				wm->workspaces[i + 1].monitor = i;
			}
		}

		xcb_cursor_context_t* cursorContext;
		xcb_cursor_context_new(wm->connection, wm->screen, &cursorContext);
		xcb_change_window_attributes(wm->connection, wm->screen->root, XCB_CW_CURSOR, (uint32_t[]){xcb_cursor_load_cursor(cursorContext, "left_ptr")});

		xcb_change_window_attributes(wm->connection, wm->screen->root, XCB_CW_EVENT_MASK, (uint32_t[]){XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
		| XCB_EVENT_MASK_STRUCTURE_NOTIFY
		| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
		| XCB_EVENT_MASK_PROPERTY_CHANGE});
		xcb_ungrab_key(wm->connection, XCB_GRAB_ANY, wm->screen->root, XCB_MOD_MASK_ANY);

		//TODO: get and grab keybindings from config file
		for (int i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
			xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(wm->connection);
			xcb_keycode_t *keycode;
			keycode = (!(keysyms) ? NULL : xcb_key_symbols_get_keycode(keysyms, keys[i].keyCode));
			xcb_key_symbols_free(keysyms);

			xcb_grab_key(wm->connection, 1, wm->screen->root, keys[i].mod, *keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
		}

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
