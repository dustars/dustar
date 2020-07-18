/*
	1. Do we need a base class? Is it necessary to use polymorphism?
	2. Now the Getters are just resources leak, do we really need them?
	3. A better method to create a cube and plane for testing purpose
	4. Normal Generation doesn't work accurately
*/

#pragma once
#include <string>
#include <vector>
#include "../core/math/Math.h"
#include "glew.h"

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

class Mesh {
protected:
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
	explicit Mesh(const string& filename); //using Assimp as model loader
	virtual ~Mesh();

	void CreatePlane();
	void CreateCube();
	void CreateQuad();

	int						GetNumOfVertices()		const { return numOfVertices; }
	GLuint					GetVAO()				const { return vao; }
	GLuint					GetType()				const { return renderType; }
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
	GLuint renderType;
	unsigned numOfVertices;
	unsigned numOfIndex;

	vector<Vector3> position;
	vector<Vector3> color;
	vector<Vector2> texCoord;
	vector<Vector3> normal;
	vector<unsigned> index;

	void EnableAttribs();
	void DisableAttribs();

	void GenerateNormals();

	void BufferDataToGPU();
	void UpdateDataToGPU();
};



