#include "event.h"

#include <iostream>
#include <xcb/xcb_keysyms.h>
#include "log.h"

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
		{ XCB_NONE,           NULL }
	};

	void handleEvent(WindowManager* wm, xcb_generic_event_t* event) {
		auto func = handleFunctions.find(event->response_type);
		(*func->second)(wm, event);
		log::log("this worked");
	}

	void handleMotion(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleEnter(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleDestroy(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleButtonPress(WindowManager* wm, xcb_generic_event_t* event) {
		xcb_button_press_event_t* buttonEvent = (xcb_button_press_event_t*)event;
		wm->window = buttonEvent->child; //Window variable might need to be part of wm
		xcb_configure_window(wm->connection, wm->window, XCB_CONFIG_WINDOW_STACK_MODE, (uint32_t[]){XCB_STACK_MODE_ABOVE});
		xcb_grab_pointer(wm->connection, 0, wm->screen->root, XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION_HINT, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, wm->screen->root, XCB_NONE, XCB_CURRENT_TIME);
	}

	void handleButtonRelease(WindowManager* wm, xcb_generic_event_t* event) {
		xcb_ungrab_pointer(wm->connection, XCB_CURRENT_TIME);
	}
	
	void handleKeyPress(WindowManager* wm, xcb_generic_event_t* event) {
		xcb_key_press_event_t* keyEvent = (xcb_key_press_event_t*)event;
		xcb_key_symbols_t* keysyms = xcb_key_symbols_alloc(wm->connection);
		xcb_keysym_t keysym;
		keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keyEvent->detail, 0));
		xcb_key_symbols_free(keysyms);
		wm->window = keyEvent->child;
		if ((keysym == 0x0078) && (keyEvent->state == XCB_MOD_MASK_4 | XCB_MOD_MASK_SHIFT)) {
			close(wm);
		}
	}

	void handleMapRequest(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleFocusIn(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleFocusOut(WindowManager* wm, xcb_generic_event_t* event) {
	}
}
