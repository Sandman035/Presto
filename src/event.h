#pragma once

#include <xcb/xcb.h>

#include "window_manager.h"

#include <unordered_map>

typedef void(*func)(presto::WindowManager*, xcb_generic_event_t*);

namespace presto {
	void handleEvent(WindowManager* wm, xcb_generic_event_t* event);

	void handleMotion(WindowManager* wm, xcb_generic_event_t* event);
	void handleEnter(WindowManager* wm, xcb_generic_event_t* event);
	void handleDestroy(WindowManager* wm, xcb_generic_event_t* event);
	void handleButtonPress(WindowManager* wm, xcb_generic_event_t* event);
	void handleButtonRelease(WindowManager* wm, xcb_generic_event_t* event);
	void handleKeyPress(WindowManager* wm, xcb_generic_event_t* event);
	void handleMapRequest(WindowManager* wm, xcb_generic_event_t* event);
	void handleFocusIn(WindowManager* wm, xcb_generic_event_t* event);
	void handleFocusOut(WindowManager* wm, xcb_generic_event_t* event);
}
