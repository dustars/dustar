#pragma once

#include <iostream>
#include <GL/glew.h>

class Debug
{
public:
	void FBOCompletenessCheck();
	void FormatCheck(); //Incomplete

	void OpenGLStatusCheck();
};

