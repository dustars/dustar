/*
	Description:
	Resource base class

	Created:
	2020/12/10
*/

#pragma once
#include <string>

class ResourceBase
{
public:


protected:
	std::string name;
	std::string path;

	unsigned int handle;
	unsigned int refCount;
//path?
};

