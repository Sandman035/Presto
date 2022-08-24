#include "event.h"

#include <iostream>
#include <xcb/xcb_keysyms.h>
#include "log.h"

#include <sys/wait.h>

namespace presto {
	std::unordered_map<uint8_t, func> handleFunctions {
		{ XCB_MOTION_NOTIFY,  handleMotion },
		{ XCB_ENTER_NOTIFY,   handleEnter },
		{ XCB_DESTROY_NOTIFY, handleDestroy },
		{ XCB_BUTTON_PRESS,   handleButtonPress },
		{ XCB_BUTTON_RELEASE, handleButtonRelease },
		{ XCB_KEY_PRESS,      handleKeyPress },
		{ XCB_MAP_REQUEST,    handleMapRequest },
		{ XCB_FOCUS_IN,       handleFocusIn },
		{ XCB_FOCUS_OUT,      handleFocusOut },
	};

	void handleEvent(WindowManager* wm, xcb_generic_event_t* event) {
		auto func = handleFunctions.find(event->response_type);
		if (func != handleFunctions.end()) {
			(*func->second)(wm, event);
		}
	}

	void handleMotion(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Motion");
		xcb_query_pointer_cookie_t coord = xcb_query_pointer(wm->connection, wm->screen->root);
		xcb_query_pointer_reply_t* pointer = xcb_query_pointer_reply(wm->connection, coord, 0);
		if (wm->value == 1 && wm->window != 0) {
			xcb_configure_window(wm->connection, wm->window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, (uint32_t[]){pointer->root_x - wm->offsetX, pointer->root_y - wm->offsetY});

			int monitor = getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors);
			if (wm->currentMonitor != monitor) {
				for (int i = 0; i < sizeof(wm->workspaces)/sizeof(wm->workspaces[0]); i++) {
					if (wm->workspaces[i].windows.empty()) 
						continue;
					wm->workspaces[i].windows.remove(wm->window);
				}
				wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.push_back(wm->window);
				wm->workspaces[wm->monitors[monitor].currentWorkspace].active = true;

				if (wm->workspaces[wm->monitors[wm->currentMonitor].currentWorkspace].windows.empty()) {
					wm->workspaces[wm->monitors[wm->currentMonitor].currentWorkspace].active = true;
				}
			}
		} else if (wm->value == 3 && wm->window != 0) {
			xcb_get_geometry_cookie_t geoCookie = xcb_get_geometry(wm->connection, wm->window);
			xcb_get_geometry_reply_t* geometry = xcb_get_geometry_reply(wm->connection, geoCookie, 0);
			if (!((pointer->root_x <= geometry->x) || (pointer->root_y <= geometry->y))) {
				xcb_configure_window(wm->connection, wm->window, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, (uint32_t[]){
						(uint32_t)pointer->root_x - geometry->x - 1 + wm->offsetX2, (uint32_t)pointer->root_y - geometry->y - 1 + wm->offsetY2});
				// TODO: Resize from corner closest to cursor
			}
		}
	}

	void handleEnter(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Enter");
		xcb_enter_notify_event_t* enterEvent = (xcb_enter_notify_event_t*) event;
		if (enterEvent->event != 0 && enterEvent->event != wm->screen->root) {
			xcb_set_input_focus(wm->connection, XCB_INPUT_FOCUS_POINTER_ROOT, enterEvent->event, XCB_CURRENT_TIME);
		}
	}

	void handleDestroy(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Destroy");
		xcb_destroy_notify_event_t* destroyEvent = (xcb_destroy_notify_event_t*) event;
		xcb_kill_client(wm->connection, destroyEvent->window); 
		for (int i = 0; i < sizeof(wm->workspaces)/sizeof(wm->workspaces[0]); i++) {
			if (wm->workspaces[i].windows.empty()) 
				continue;
			wm->workspaces[i].windows.remove(destroyEvent->window);
		}
	}

	void handleButtonPress(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Button Press");
		xcb_button_press_event_t* buttonEvent = (xcb_button_press_event_t*)event;
		if (buttonEvent->child != 0 && buttonEvent->child != wm->screen->root) {
			wm->window = buttonEvent->child;
			xcb_configure_window(wm->connection, wm->window, XCB_CONFIG_WINDOW_STACK_MODE, (uint32_t[]){XCB_STACK_MODE_ABOVE});
			wm->value = ((1 == buttonEvent->detail) ? 1 : ((wm->window != 0) ? 3 : 0));
			xcb_grab_pointer(wm->connection, 0, wm->screen->root,
					XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION_HINT,
					XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE,
					XCB_CURRENT_TIME);

			xcb_query_pointer_cookie_t coord = xcb_query_pointer(wm->connection, wm->screen->root);
			xcb_query_pointer_reply_t* pointer = xcb_query_pointer_reply(wm->connection, coord, 0);
			xcb_get_geometry_cookie_t geoCookie = xcb_get_geometry(wm->connection, wm->window);
			xcb_get_geometry_reply_t* geometry = xcb_get_geometry_reply(wm->connection, geoCookie, 0);
			wm->offsetX = pointer->root_x - geometry->x;
			wm->offsetY = pointer->root_y - geometry->y;
			wm->offsetX2 = geometry->width - wm->offsetX;
			wm->offsetY2 = geometry->height - wm->offsetY;

			wm->currentMonitor = getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors);
		}
	}

	void handleButtonRelease(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Button Release");
		xcb_ungrab_pointer(wm->connection, XCB_CURRENT_TIME);
	}
	
	void handleKeyPress(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Key");
		xcb_key_press_event_t* keyEvent = (xcb_key_press_event_t*)event;
		xcb_key_symbols_t* keysyms = xcb_key_symbols_alloc(wm->connection);
		xcb_keysym_t keysym;
		keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keyEvent->detail, 0));
		xcb_key_symbols_free(keysyms);
		wm->window = keyEvent->child;
		if ((keysym == 0x0078) && (keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT)) {
			close(wm);
		} else if ((keysym == 0x0070) && (keyEvent->state == XCB_MOD_MASK_4)) {
			if (fork() == 0) {
				setsid();
				if (fork() != 0) {
					_exit(0);
				}
				execvp("dmenu_run", NULL);
				_exit(0);
			}
			wait(NULL);
		} else if (keysym >= 0x0030 && keysym <= 0x0039 && keyEvent->state == XCB_MOD_MASK_4) {
			changeWorkspace(wm, keysym - 48);
		} else if (keysym == 0x0068 && keyEvent->state == XCB_MOD_MASK_4) {
			changeWorkspace(wm, wm->keyWorkspaces[0]);
		} else if (keysym == 0x006a && keyEvent->state == XCB_MOD_MASK_4) {
			changeWorkspace(wm, wm->keyWorkspaces[1]);
		} else if (keysym == 0x006b && keyEvent->state == XCB_MOD_MASK_4) {
			changeWorkspace(wm, wm->keyWorkspaces[2]);
		} else if (keysym == 0x006c && keyEvent->state == XCB_MOD_MASK_4) {
			changeWorkspace(wm, wm->keyWorkspaces[3]);
		} else if (keysym == 0x0068 && keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT) {
			wm->keyWorkspaces[0] = wm->monitors[getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors)].currentWorkspace;
		} else if (keysym == 0x006a && keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT) {
			wm->keyWorkspaces[1] = wm->monitors[getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors)].currentWorkspace;
		} else if (keysym == 0x006b && keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT) {
			wm->keyWorkspaces[2] = wm->monitors[getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors)].currentWorkspace;
		} else if (keysym == 0x006c && keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT) {
			wm->keyWorkspaces[3] = wm->monitors[getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors)].currentWorkspace;
		}
	}

	void handleMapRequest(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Map");
		xcb_map_request_event_t* mapEvent = (xcb_map_request_event_t*)event;
		xcb_map_window(wm->connection, mapEvent->window);
	
		int monitor = getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors);
		wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.push_back(mapEvent->window);
		wm->workspaces[wm->monitors[monitor].currentWorkspace].active = true;
		xcb_configure_window(wm->connection,
				mapEvent->window,
				XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_BORDER_WIDTH,
				(uint32_t[]){
				(uint32_t)wm->monitors[monitor].x + (wm->monitors[monitor].width / 2) - (800 / 2),
				(uint32_t)wm->monitors[monitor].y + (wm->monitors[monitor].height	/ 2) - (600 / 2),
				800,
				600,
				1});

		xcb_flush(wm->connection);
		xcb_change_window_attributes_checked(wm->connection, mapEvent->window, XCB_CW_EVENT_MASK, (uint32_t[]){XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE});
		if (mapEvent->window != wm->screen->root) {
			xcb_set_input_focus(wm->connection, XCB_INPUT_FOCUS_POINTER_ROOT, mapEvent->window, XCB_CURRENT_TIME);
		}
	}

	void handleFocusIn(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Focus In");
		xcb_focus_in_event_t* focusEvent = (xcb_focus_in_event_t*)event;
		if (focusEvent->event != 0 && focusEvent->event != wm->screen->root) {
			xcb_change_window_attributes(wm->connection, focusEvent->event, XCB_CW_BORDER_PIXEL, (uint32_t[]){0xFFFFFF});
			xcb_flush(wm->connection);
		}
	}

	void handleFocusOut(WindowManager* wm, xcb_generic_event_t* event) {
		log::log("Focus Out");
		xcb_focus_in_event_t* focusEvent = (xcb_focus_in_event_t*)event;
		if (focusEvent->event != 0 && focusEvent->event != wm->screen->root) {
			xcb_change_window_attributes(wm->connection, focusEvent->event, XCB_CW_BORDER_PIXEL, (uint32_t[]){0x696969});
			xcb_flush(wm->connection);
		}
	}
}
