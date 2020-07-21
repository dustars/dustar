/*
	Description:
	Currently under construction.
	Now it checks FBO completeness.

	Last updated:
	2020/7/21

	TODO:
	1.	If it's possible to integrate the text renderer into the debugger?
		Debugger will need to pass the string argument to text renderer again
		from outside, which might be acceptable if using move semantics?
*/

#pragma once
#include <iostream>
#include <GL/glew.h>
#include "TextRenderer.h"

class Debug
{
public:
	Debug() {}
	~Debug() {}

	//Check the FBO completeness
	void FBOCompletenessCheck();

	//Check if the texture is complete
	void FormatCheck(); //Incomplete

	void OpenGLStatusCheck();
};

