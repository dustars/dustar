/*
	1. Do we need a base class? Now it's really class-based programming. Is it necessary to use polymorphism?
	2. Now the Getters are just resources leak, do we really need them?
*/

#pragma once
#include <string>
#include <vector>
#include "../basic/math/Vector4.h"
#include "../basic/math/Vector2.h"
#include "glew.h"

using namespace std;

class Mesh {
	enum BufferName {
		POSITION,
		COLOR,
		TEXTURE,
		NORMAL,
		INDEX,
		MAXBUFFER
	};

public:
	Mesh();
	Mesh(string filename);
	~Mesh();

	void CreateTriangle();
	void CreateCube();
	void CreateQuad();

	int						GetNumOfVertices()		{ return numOfVertices; }
	GLuint					GetVAO()				{ return vao; }
	GLuint					GetType()				{ return renderType; }
	vector<Vector3>&		GetPosition()			{ return position; }
	vector<Vector3>&		GetColor()				{ return color; }
	vector<Vector2>&		GetTexCoord()			{ return texCoord; }
	vector<Vector3>&		GetNormal()				{ return normal; }
	vector<unsigned>&		GetIndex()				{ return index; }

	virtual void Draw();
	virtual void Update(float dt);
	
protected:
	GLuint vao;
	GLuint vbo[MAXBUFFER];
	void BufferDataToGPU();
	void UpdateDataToGPU();

	GLuint renderType;

	vector<Vector3> position;
	vector<Vector3> color;
	vector<Vector2> texCoord;
	vector<Vector3> normal;
	vector<unsigned> index;

	unsigned numOfVertices;
	unsigned numOfIndex;

	void EnableAttribs();
	void DisableAttribs();

	void GenerateNormals();
};



