#pragma once

// Transplant from the NCLGL Graphics module

#include <string>
#include <fstream>
#include <vector>

#include "glew.h"
#include "wglew.h"

//Math
#include "math/Vector4.h"
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "math/Quaternion.h"
#include "math/Matrix4.h"

//Basic funtionalities
#include "Window.h"

using namespace std;
using std::vector;

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")

class RenderBase
{
public:
	friend class Window;
	RenderBase(Window& parent);
	virtual ~RenderBase(void);

	virtual void	RenderScene() = 0;

	//Returns TRUE if everything in the constructor has gone to plan.
	inline bool HasInitialised() const{ return init; }

protected:
	virtual void	Resize(int x, int y);

	int		width;			//Render area width (not quite the same as window width)
	int		height;			//Render area height (not quite the same as window height)
	bool	init;			//Did the renderer initialise properly?

	HDC		deviceContext;	//...Device context?
	HGLRC	renderContext;	//Permanent Rendering Context
};

