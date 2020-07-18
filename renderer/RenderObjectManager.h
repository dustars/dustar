#pragma once
#include "RenderObject.h"

class RenderObjectManager
{
public:
	//Contain all renderable objects
	vector<RenderObject> objectList;

	RenderObjectManager();
	~RenderObjectManager();

	void AddRenderObject(RenderObject* object);
};

