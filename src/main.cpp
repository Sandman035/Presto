#include <iostream>
#include "window_manager.h"
#include "log.h"

#include <unistd.h>

int main() {
	log::init();
	
	presto::WindowManager wm;

	if (presto::connect(&wm)) {
		return 1;
	}

	log::log("connected to the x server");

	while (!wm.shouldClose()) {
		presto::run(&wm);
	}

	presto::disconnect(&wm);

	log::shutdown();
}
