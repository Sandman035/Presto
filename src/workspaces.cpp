#include "workspaces.h"

#include "window_manager.h"
#include <algorithm>

namespace presto {
	void changeWorkspace(WindowManager* wm, int number) {
		int monitor = getMonitorUnderCursor(wm->connection, wm->screen->root, wm->monitors);
		if (wm->monitors[monitor].currentWorkspace != number) {
			std::list<xcb_window_t>::iterator it;
			if (wm->workspaces[number].active && wm->workspaces[number].monitor != monitor) {
				if (wm->monitors[wm->workspaces[number].monitor].currentWorkspace != number) {
					if (!wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.empty()) {
						for (it = wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.begin(); it != wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].windows.end(); ++it) {
							xcb_unmap_window(wm->connection, *it);
						}
					} else {
						wm->workspaces[wm->monitors[wm->workspaces[number].monitor].currentWorkspace].active = false;
					}
				}
				xcb_warp_pointer(wm->connection, XCB_NONE, wm->screen->root, 0, 0, 0, 0, wm->monitors[wm->workspaces[number].monitor].width / 2 + wm->monitors[wm->workspaces[number].monitor].x, wm->monitors[wm->workspaces[number].monitor].height / 2 + wm->monitors[wm->workspaces[number].monitor].y);
				xcb_flush(wm->connection);
				wm->monitors[wm->workspaces[number].monitor].currentWorkspace = number;
			} else {
				for (it = wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.begin(); it != wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.end(); ++it) {
					xcb_unmap_window(wm->connection, *it);
				}

				wm->monitors[monitor].currentWorkspace = number;
				wm->workspaces[number].monitor = monitor;
				
				if (wm->workspaces[wm->monitors[monitor].currentWorkspace].windows.empty()) {
					wm->workspaces[wm->monitors[monitor].currentWorkspace].active = false;
				}
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

			if (wm->workspaces[i].windows.empty()) {
				wm->workspaces[i].active = false;
			}
		}

		wm->workspaces[number].windows.push_back(wm->window);
		wm->workspaces[number].active = true;

	}
}
