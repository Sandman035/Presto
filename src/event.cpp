#include "event.h"

#include <iostream>

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
	}

	void handleMotion(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleEnter(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleDestroy(WindowManager* wm, xcb_generic_event_t* event) {
		std::cout << "this worked\n";
		presto::close(wm);
	}

	void handleButtonPress(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleButtonRelease(WindowManager* wm, xcb_generic_event_t* event) {
	}
	
	void handleKeyPress(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleMapRequest(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleFocusIn(WindowManager* wm, xcb_generic_event_t* event) {
	}

	void handleFocusOut(WindowManager* wm, xcb_generic_event_t* event) {
	}
}
