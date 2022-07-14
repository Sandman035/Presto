#include "monitors.h"

#include <xcb/xcb.h>
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

	int getMonitorUnderCursor(xcb_connection_t* connection, xcb_window_t root, std::vector<Monitor> monitors) {
		xcb_query_pointer_cookie_t coord = xcb_query_pointer(connection, root);
		xcb_query_pointer_reply_t* pointer = xcb_query_pointer_reply(connection, coord, 0);
		for (int i = 0; i < monitors.size(); i++) {
			if (pointer->root_x > monitors[i].x && pointer->root_x < monitors[i].x + monitors[i].width && pointer->root_y > monitors[i].y && pointer->root_y < monitors[i].y + monitors[i].height) {
				return i;
			}
		}
		return 0;
	}
}
