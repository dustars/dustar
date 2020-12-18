#include "AssimpModel.h"

AssimpModel::AssimpModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/')); //what does it do?

	ProcessNode(scene->mRootNode, scene);

    for (auto& ele : meshes) {
        ele.BufferDataToGPU();
    }
}

void AssimpModel::Draw()
{
    for (std::size_t i = 0; i < meshes.size(); i++) {
        for (std::size_t j = 0; j < materialsIndex[i].size(); j++) {
            glBindTextureUnit(10 + j, materials[materialsIndex[i][j]].GetTexture());
        }

        meshes[i].Draw();
    }
}

void AssimpModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene); //Should use move semantics
        meshCount++;
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

void AssimpModel::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    Mesh ourMesh;
    // process vertex
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        ourMesh.GetPosition().push_back(Vector3(mesh->mVertices[i].x,mesh->mVertices[i].y, mesh->mVertices[i].z));
        ourMesh.GetNormal().push_back(Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
       
        if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
            ourMesh.GetTexCoord().push_back(Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }else{
            ourMesh.GetTexCoord().push_back(Vector2(0.f, 0.f));
        }
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            ourMesh.GetIndex().push_back(face.mIndices[j]);
        }

    }

    meshes.push_back(ourMesh);

    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
        {
            aiString str;
            material->GetTexture(aiTextureType_DIFFUSE, i, &str);
            std::string filePath = directory + "/" + str.C_Str();
            //Using file path to uniquely identify if the texture is already loaded
            auto texIndex = std::find_if(materials.begin(), materials.end(),
                                         [&filePath](Texture& b) {return filePath == b.GetPath();});
            if (texIndex != materials.end()) {
                materialsIndex[meshCount].push_back(texIndex - materials.begin());
            } else {
                Texture texture;
                if (!texture.SetTexture(filePath)) {
                    std::cout << "AssimpModel Initilization: Texture setup failed! " << std::endl;
                }
                texture.SetTextureType(Texture::TextureType::DIFFUSE);

                materialsIndex[meshCount].push_back(materials.end() - materials.begin());
                materials.push_back(texture);
            }
        }

        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
        {
            aiString str;
            material->GetTexture(aiTextureType_SPECULAR, i, &str);
            std::string filePath = directory + "/" + str.C_Str();
            //Using file path to uniquely identify if the texture is already loaded
            auto texIndex = std::find_if(materials.begin(), materials.end(),
                                         [&filePath](Texture& b) {return filePath == b.GetPath();});
            if (texIndex != materials.end()) {
                materialsIndex[meshCount].push_back(texIndex - materials.begin());
            }
            else {
                Texture texture;
                if (!texture.SetTexture(filePath)) {
                    std::cout << "AssimpModel Initilization: Texture setup failed! " << std::endl;
                }
                texture.SetTextureType(Texture::TextureType::SPECULAR);

                materialsIndex[meshCount].push_back(materials.end() - materials.begin());
                materials.push_back(texture);
            }
        }
    }
}