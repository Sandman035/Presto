#include "workspaces.h"

#include "window_manager.h"
#include <algorithm>

namespace presto {
	void changeWorkspace(WindowManager* wm, int number) {
		int monitor = getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors);

		if (wm->monitors[monitor].currentWorkspace != number) { //If the current workspace is not the wanted workspace
			std::list<xcb_window_t>::iterator it;

			if (wm->workspaces[number].active && wm->workspaces[number].monitor != monitor) { //If the workspace is active and the workspace is on a different monitor
				if (wm->monitors[wm->workspaces[number].monitor].currentWorkspace != number) { //If the current workspace of the other monitor is not the wanted workspace
					if (!wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.empty()) { //If the current monitor of the other monitor is not empty
						for (it = wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.begin(); it != wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.end(); ++it) {
							xcb_unmap_window(wm->connection, *it);
						}
					} else {
						wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].active = false;
					}
				}

				//Move cursor to the other monitor
				xcb_warp_pointer(wm->connection, XCB_NONE, wm->screen->root, 0, 0, 0, 0, wm->monitors[wm->workspaces[number].monitor].width / 2 + wm->monitors[wm->workspaces[number].monitor].x, wm->monitors[wm->workspaces[number].monitor].height / 2 + wm->monitors[wm->workspaces[number].monitor].y);
				xcb_flush(wm->connection);
				wm->monitors[wm->workspaces[number].monitor].currentWorkspace = number; //make the current workspace of the monitor the wanted workspace

			} else { //If the wanted workspace is not active or its on this monitor

				//Unmap all windows of the current workspace
				for (it = wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.begin(); it != wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.end(); ++it) {
					xcb_unmap_window(wm->connection, *it);
				}

				if (wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.empty()) {
					wm->workspaces[wm->monitors[monitor].currentWorkspace].active = false;
				}

				wm->monitors[monitor].currentWorkspace = number;
				wm->workspaces[number].monitor = monitor;
			}

			for (it = wm->workspaces[number].windows.begin(); it != wm->workspaces[number].windows.end(); ++it) {
				xcb_map_window(wm->connection, *it);
			}
			wm->workspaces[number].active = true;
		}
	}

	void moveToWorkspace(WindowManager* wm, xcb_window_t window, int number) {
		for (int i = 0; i < sizeof(wm->workspaces)/sizeof(wm->workspaces[0]); i++) {
			if (wm->workspaces[i].windows.empty()) 
				continue;
			wm->workspaces[i].windows.remove(window);

			if (wm->workspaces[i].windows.empty() && wm->monitors[wm->workspaces[i].monitor].currentWorkspace != i) {
				wm->workspaces[i].active = false;
			}
		}

		wm->workspaces[number].windows.push_back(wm->window);
		wm->workspaces[number].active = true;

	}
}
