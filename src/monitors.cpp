#include "monitors.h"

#include <xcb/randr.h>

namespace presto {
	std::vector<Monitor> getMonitors(xcb_connection_t* connection, xcb_window_t window) {
		std::vector<Monitor> monitors;

		xcb_randr_get_screen_resources_current_cookie_t resourcesCookie = xcb_randr_get_screen_resources_current(connection, window);
		xcb_randr_get_screen_resources_current_reply_t* resources = xcb_randr_get_screen_resources_current_reply(connection, resourcesCookie, 0);

		int length = xcb_randr_get_screen_resources_current_outputs_length(resources);
		xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_current_outputs(resources);

		for (int i = 0; i < length; i++) {
			xcb_randr_get_output_info_cookie_t infoCookie = xcb_randr_get_output_info(connection, outputs[i], XCB_CURRENT_TIME);
			xcb_randr_get_output_info_reply_t* output = xcb_randr_get_output_info_reply(connection, infoCookie, 0);

			if (output == NULL)
				continue;

			if (output->crtc == XCB_NONE || output->connection == XCB_RANDR_CONNECTION_DISCONNECTED)
				continue;

			xcb_randr_get_crtc_info_cookie_t crtcCookie = xcb_randr_get_crtc_info(connection, output->crtc, XCB_CURRENT_TIME);
			xcb_randr_get_crtc_info_reply_t* crtc = xcb_randr_get_crtc_info_reply(connection, crtcCookie, 0);

			Monitor monitor;
			monitor.height = crtc->height;
			monitor.width = crtc->width;
			monitor.x = crtc->x;
			monitor.y = crtc->y;

			monitors.push_back(monitor);
		}

		return monitors;
	}
}
