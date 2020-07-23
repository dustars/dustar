#pragma comment(lib, "core.lib")

#include "window.h"
#include "Renderer.h"

int main() {

	Window w("OpenGL", 1280, 720, false); // -16 for viewport size, 528 for 512 dimension

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

#ifdef OFFLINE
	renderer.TestRendering();
#else
	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		renderer.Update(w.GetTimer()->GetTimedMS());

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) renderer.ScreenShot(); // ScreenShot, saved in "demo/screenshot.jpg"
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_T)) renderer.SwitchIsRenderingText();
	}
#endif
	return 0;
}