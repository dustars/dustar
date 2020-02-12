#pragma comment(lib, "basic.lib")

#include "window.h"
#include "Renderer.h"

int main() {

	Window w("OpenGL", 1000, 1000, false);

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.startingTime = w.GetTimer()->GetMS()/1000;
		renderer.RenderScene();
	}

	return 0;
}