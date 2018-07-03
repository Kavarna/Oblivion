
// C / C++
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstdarg>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/types.h>

// Oblivion
#include <OblivionObjects.h>

using namespace Oblivion;

void PrintHelp()
{
	std::cout << "default args: -tan 1" << std::endl;
	std::cout << "-i [FILE] = file to be converted\n"\
		"-o [FILE] = output file\n"\
		"-flags [NUMBER] = flags\n";
}

void InitMesh(const aiMesh * pMesh, std::vector<aiVector3D>& positions,
	std::vector<aiVector2D>& texCoords, std::vector<aiVector3D>& normals,
	std::vector<aiVector3D>& tangents, std::vector<aiVector3D>& bitangents,
	std::vector<unsigned int>& indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
	{
		const aiVector3D* pTexCoord = pMesh->HasTextureCoords(0) ? &(pMesh->mTextureCoords[0][i]) : &Zero3D;
		texCoords.emplace_back(pTexCoord->x,pTexCoord->y);
		
		positions.push_back(pMesh->mVertices[i]);
		normals.push_back(pMesh->mNormals[i]);
		tangents.push_back(pMesh->mTangents[i]);
		bitangents.push_back(pMesh->mBitangents[i]);
	}

	for (unsigned int i = 0; i < pMesh->mNumFaces; ++i)
	{
		const aiFace& face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
}

void InitMaterials(const aiScene * scene, std::vector<material_t>& materials)
{
	materials.reserve(scene->mNumMaterials);

	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial * pMaterial = scene->mMaterials[i];
		aiColor4D diffuseColor;
		pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		materials[i].diffuseColor.x = diffuseColor.r;
		materials[i].diffuseColor.y = diffuseColor.g;
		materials[i].diffuseColor.z = diffuseColor.b;

		if (pMaterial->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			pMaterial->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			materials[i].diffuseMap = std::string(path.data);
		}
		if (pMaterial->GetTextureCount(aiTextureType::aiTextureType_NORMALS) > 0)
		{
			aiString path;
			pMaterial->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &path);
			materials[i].bumpMap = std::string(path.data);
		}
		if (pMaterial->GetTextureCount(aiTextureType::aiTextureType_SPECULAR) > 0)
		{
			aiString path;
			pMaterial->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
			materials[i].specularMap = std::string(path.data);
		}
	}
}

void InitFromScene(const aiScene * scene, std::string const& from, std::string const& to)
{
	std::vector<material_t> materials(scene->mNumMaterials);
	std::vector<mesh_t> meshes(scene->mNumMeshes);

	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		meshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].baseVertex = numVertices;
		meshes[i].baseIndex = numIndices;

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	std::vector<aiVector3D> positions;
	std::vector<aiVector2D> texCoords;
	std::vector<aiVector3D> normals;
	std::vector<aiVector3D> tangents;
	std::vector<aiVector3D> bitangents;
	std::vector<uint32_t> indices;

	positions.reserve(numVertices);
	texCoords.reserve(numVertices);
	normals.reserve(numVertices);
	tangents.reserve(numVertices);
	bitangents.reserve(numVertices);
	
	indices.reserve(numIndices);

	for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
	{
		const aiMesh* pMesh = scene->mMeshes[i];
		InitMesh(pMesh, positions, texCoords, normals, tangents, bitangents, indices);
	}

	InitMaterials(scene, materials);

}

void PrintFromScene(const aiScene * pScene, std::string const& to)
{
	using namespace std;
	ofstream file(to.c_str());
	streambuf * coutbuf = std::cout.rdbuf();
	cout.rdbuf(file.rdbuf());

	if (!pScene->HasAnimations())
		cout << "Static\n";
	else
		cout << "Animation\n";

	cout << "Meshes " << pScene->mNumMeshes << " { "<< std::endl;

	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		const aiMesh * mesh = pScene->mMeshes[i];
		
#pragma region Check and print vertices
		std::string name;
		for (unsigned int i = 0; i < mesh->mName.length; ++i)
		{
			if (mesh->mName.data[i] != ' ')
				name += mesh->mName.data[i];
		}
		cout << "\t" << name << " {" << std::endl;
		cout << "\t\tVertices " << mesh->mNumVertices << " ";
		bool hasTexCoord, hasNormals, hasOtherVectors;

		if (mesh->HasTextureCoords(0))
		{
			cout << "UV 1 ";
			hasTexCoord = true;
		}
		else
		{
			cout << "UV 0 ";
			hasTexCoord = false;
		}

		if (mesh->HasNormals())
		{
			cout << "Normals 1 ";
			hasNormals = true;
		}
		else
		{
			cout << "Normals 0 ";
			hasNormals = false;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			cout << "Others 1 ";
			hasOtherVectors = true;
		}
		else
		{
			cout << "Others 0 ";
			hasOtherVectors = false;
		}

		cout << " {\n";
		
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			cout << "\t\t\t" << mesh->mVertices[i].x << ' ' << mesh->mVertices[i].y << ' ' << mesh->mVertices[i].z << ' ';
			if (hasTexCoord)
				cout << mesh->mTextureCoords[0][i].x << ' ' << mesh->mTextureCoords[0][i].y << ' ';
			if (hasNormals)
				cout << mesh->mNormals[i].x << ' ' << mesh->mNormals[i].y << ' ' << mesh->mNormals[i].z << ' ';
			if (hasOtherVectors)
				cout << mesh->mTangents[i].x << ' ' << mesh->mNormals[i].y << ' ' << mesh->mNormals[i].z << ' ' \
				<< mesh->mBitangents[i].x << ' ' << mesh->mBitangents[i].y << ' ' << mesh->mBitangents[i].z << ' ';
			cout << '\n';
		}

		cout << "\t\t}\n";
#pragma endregion

#pragma region Print indices

		cout << "\t\tIndices " << mesh->mNumFaces * 3 << " {\n";

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			const aiFace& face = mesh->mFaces[i];
			cout << "\t\t\t" << face.mIndices[0] << ' '
				<< face.mIndices[1] << ' ' << face.mIndices[2] << '\n';
		}

		cout << "\t\t}\n";

#pragma endregion

		cout << "\t\tMaterial " << mesh->mMaterialIndex << "\n";

		cout << "\t}\n";
	}

	cout << "}\n";

	cout << "Materials " << pScene->mNumMaterials << " { " << std::endl;

	for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
	{
		const aiMaterial* material = pScene->mMaterials[i];
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		cout << "\t" << name.data << " {\n";

		if (material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			cout << "\t\t\tTexture " << path.data << "\n";
		}
		else
		{
			aiColor4D color;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			cout << "\t\t\tColor " << color.r << ' ' <<
				color.g << ' ' << color.b << ' ' << color.a << '\n';
		}

		if (material->GetTextureCount(aiTextureType::aiTextureType_NORMALS) > 0)
		{
			aiString path;
			material->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &path);
			cout << "\t\t\tBump " << path.data << "\n";
		}

		float specPower;
		if (!material->Get(AI_MATKEY_SHININESS, specPower))
		{
			cout << "\t\t\tShininess " << specPower << "\n";
			if (material->GetTextureCount(aiTextureType::aiTextureType_SPECULAR) > 0)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
				cout << "\t\t\tSpecular " << path.data << "\n";
			}
		}

		float opacity;
		if (!material->Get(AI_MATKEY_OPACITY, opacity))
		{
			cout << "\t\t\tOpacity " << opacity << "\n";
		}

		cout << "\t}\n";
	}

	cout << "}\n";

	cout.rdbuf(coutbuf);
	file.close();
}

void ConvertFile(std::string const& from, std::string const& to, unsigned int flags)
{
	Assimp::Importer importer;
	
	const aiScene * pScene = importer.ReadFile(from.c_str(), flags);
	//InitFromScene(pScene, from, to);
	PrintFromScene(pScene, to);
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::cout << "use -help for help";
		std::cin.get();
	}
	std::string inputFile = "NOTHING";
	std::string outputFile = "NOTHING";
	unsigned int flags = (aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace);
	for (int i = 1; i < argc; ++i) // argv[0] is useless
	{
		if (!strcmp(argv[i], "-help"))
			PrintHelp();
		if (!strcmp(argv[i], "-i"))
		{
			inputFile = std::string(argv[++i]);
			std::cout << "inputFile = " << inputFile << std::endl;
			if (!std::ifstream(inputFile.c_str()))
			{
				std::cout << "Can't open file " << inputFile << "\n";
				return 1;
			}
		}
		if (!strcmp(argv[i], "-o"))
		{
			outputFile = std::string(argv[++i]);
			std::cout << "outputFile = " << outputFile << std::endl;
		}
		if (!strcmp(argv[i], "-flags"))
		{
			flags = atoi(argv[++i]);
		}
	}
	if (inputFile == "NOTHING" || outputFile == "NOTHING")
	{
		std::cout << "Can't make that conversion" << std::endl;
		return 1;
	}
	ConvertFile(inputFile, outputFile, flags);
}