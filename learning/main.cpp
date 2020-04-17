#pragma comment(lib, "basic.lib")

#include "window.h"
#include "Renderer.h"

int main() {

	Window w("OpenGL", 1280, 720, false);

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		float dt = w.GetTimer()->GetTimedMS();
		renderer.Update(dt);
	}

	return 0;
}