#include <iostream>
#include "window_manager.h"

int main() {
	presto::WindowManager wm;

	if (presto::connect(&wm)) {
		return 1;
	}

	std::cout << "Connected to x server\n";

	int i = 0;
	while (!wm.shouldClose()) {
		if (i >= 1000000000)
			presto::close(&wm);

		i++;
	}

	presto::disconnect(&wm);
}
