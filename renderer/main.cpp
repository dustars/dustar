#include "window.h"
#include "Renderer.h"

#pragma comment(lib, "core.lib")

extern "C" {
	#include "../Lua540_x86/include/lua.h"
	#include "../Lua540_x86/include/lauxlib.h"
	#include "../Lua540_x86/include/lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "../Lua540_x86/liblua54.a")
#endif

bool CheckLua(lua_State* L, int r) {
	if (r != LUA_OK) {
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}

int main() {

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	if (CheckLua(L, luaL_dofile(L, "LuaScripts/example.lua"))) {
	}

	//1043, 1063 for square
	Window w("OpenGL", 1280, 720, false); // -16 for viewport size, 528 for 512 dimension

	if (!w.HasInitialised()) return -1;

	Renderer renderer(w);

	if (!renderer.HasInitialised()) return -1;

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