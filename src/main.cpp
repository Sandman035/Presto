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

	int i = 0;
	while (!wm.shouldClose()) {
		//presto::run(&wm);

		if (i >= 100)
			presto::close(&wm);

		i++;
	}

	presto::disconnect(&wm);

	log::shutdown();
}
