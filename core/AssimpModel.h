/*
	Description:
	AssimpModel class

	Modified From: https://learnopengl.com/AssimpModel-Loading/AssimpModel
*/

#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Mesh.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AssimpModel
{
public:
	AssimpModel(const std::string& path);

	void Draw();

private:
	typedef std::size_t meshIndex;

	std::vector<Mesh> meshes;
	std::unordered_map<meshIndex, std::vector<std::size_t>> materialsIndex;
	std::vector<Texture> materials;
	std::string directory;
	meshIndex meshCount = 0;

	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

