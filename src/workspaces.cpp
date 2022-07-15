#include "workspaces.h"

#include "window_manager.h"

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
}
