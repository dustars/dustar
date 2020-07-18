/*
	Transplant from the NCLGL Graphics module
*/

#pragma once
#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/wglew.h>

//basic funtionalities
#include "math/math.h"
#include "Window.h"

using namespace std;
using std::vector;

#pragma comment(lib, "opengl32.lib")

class RenderBase
{
public:
	friend class Window;
	RenderBase(Window& parent);
	virtual ~RenderBase(void);

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

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

