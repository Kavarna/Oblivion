
#ifdef _WIN32 //32 bit windows
#define ENVIRONMENT32
#if defined _WIN64 //64 bit windows
#undef ENVIRONMENT32
#define ENVIRONMENT64
#endif // _WIN64
#endif // _WIN32

#if defined ENVIRONMENT32

// C / C++
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstdarg>
#include <sstream>
#include <string>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/texture.h>
#include <assimp/types.h>

// Boost
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/lexical_cast.hpp"

// Oblivion
#include <OblivionObjects.h>

using namespace Oblivion;

struct SModel
{
	std::vector<SVertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t materialIndex;
	std::string name;

	bool hasTexture;
	bool hasNormal;
	bool hasTangent;

	SModel() = default;
};

void PrintHelp()
{
	std::cout << "default args: -tan 1" << std::endl;
	std::cout << "-i [FILE] = file to be converted\n"\
		"-o [FILE] = output file\n"\
		"-flags [NUMBER] = flags\n";
}

void CalculateTangentAndBinormal(const DirectX::XMFLOAT3& P1, const DirectX::XMFLOAT3& P2,
	const DirectX::XMFLOAT3& P3, const DirectX::XMFLOAT4& T1, const DirectX::XMFLOAT4& T2,
	const DirectX::XMFLOAT4& T3, DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal)
{
	DirectX::XMFLOAT3 edge1; DirectX::XMFLOAT2 tex1;
	DirectX::XMFLOAT3 edge2; DirectX::XMFLOAT2 tex2;

	edge1.x = P2.x - P1.x;
	edge1.y = P2.y - P1.y;
	edge1.z = P2.z - P2.z;

	edge2.x = P3.x - P1.x;
	edge2.y = P3.y - P1.y;
	edge2.z = P3.z - P1.z;

	tex1.x = T2.x - T1.x;
	tex1.y = T2.y - T1.y;

	tex2.x = T3.x - T1.x;
	tex2.y = T3.y - T1.y;

	float den = 1.0f / (tex1.x * tex2.y - tex1.y * tex2.x);

	tangent.x = (tex2.y * edge1.x - tex1.y * edge2.x) * den;
	tangent.y = (tex2.y * edge1.y - tex1.y * edge2.y) * den;
	tangent.z = (tex2.y * edge1.z - tex1.y * edge2.z) * den;
	float sum = sqrtf(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
	tangent.x /= sum;
	tangent.y /= sum;
	tangent.z /= sum;

	binormal.x = (tex1.x * edge2.x - tex2.x * edge1.x) * den;
	binormal.y = (tex1.x * edge2.y - tex2.x * edge1.y) * den;
	binormal.z = (tex1.x * edge2.z - tex2.x * edge1.z) * den;
	sum = sqrtf(binormal.x * binormal.x + binormal.y * binormal.y + binormal.z * binormal.z);
	binormal.x /= sum;
	binormal.y /= sum;
	binormal.z /= sum;
}

void WriteMaterial(const aiScene* pScene, std::string& to)
{
	using boost::property_tree::ptree;
	ptree materialTree;
	for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
	{
		//materials.put(pScene->mMaterials[i].Get,)
		auto material = pScene->mMaterials[i];
		aiString name;
		material->Get(AI_MATKEY_NAME, name);
		std::string nameStr = name.C_Str();

		ptree currentMaterialTree;

		if (material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			currentMaterialTree.put("Texture", path.C_Str());
		}
		else
		{
			aiColor4D color;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

			std::string colorString;
			colorString += boost::lexical_cast<std::string>(color.r); colorString += " ";
			colorString += boost::lexical_cast<std::string>(color.g); colorString += " ";
			colorString += boost::lexical_cast<std::string>(color.b); colorString += " ";
			colorString += boost::lexical_cast<std::string>(color.a);

			currentMaterialTree.put("Color", colorString);
		}

		if (material->GetTextureCount(aiTextureType::aiTextureType_HEIGHT) > 0)
		{
			aiString path;
			material->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &path);
			std::string bumpPath = path.C_Str();
			currentMaterialTree.put("Bump", bumpPath);
		}

		float specPower;
		if (!material->Get(AI_MATKEY_SHININESS, specPower))
		{
			//cout << "\t\t\tShininess " << specPower << "\n";
			currentMaterialTree.put("Shininess", specPower);
			if (material->GetTextureCount(aiTextureType::aiTextureType_SPECULAR) > 0)
			{
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &path);
				//cout << "\t\t\tSpecular " << path.data << "\n";
				std::string specularMap = path.C_Str();
				currentMaterialTree.put("Specular map", specularMap);
			}
		}

		float opacity;
		if (!material->Get(AI_MATKEY_OPACITY, opacity))
		{
			currentMaterialTree.put("Opacity", opacity);
		}

		currentMaterialTree.put("Tesselation.Minimum", 1.0f);
		currentMaterialTree.put("Tesselation.Maximum", 1.0f);
		currentMaterialTree.put("Tesselation.Scale", 0.1f);

		materialTree.put_child(nameStr, currentMaterialTree);
	}


	boost::property_tree::json_parser::write_json(to, materialTree);
}

void PrintFromScene(const aiScene * pScene, std::string& to)
{
	using namespace std;
	ofstream file((to + ".obl").c_str());
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

	std::string materialFile;
	materialFile = to + ".material";

	WriteMaterial(pScene, materialFile);

	cout.rdbuf(coutbuf);
	file.close();
}

void CreateModels(const aiScene* pScene, std::vector<SModel>& models)
{
	models.clear();
	models.reserve(pScene->mNumMeshes);

	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		models.emplace_back();

		SModel * model = &models.back();

		aiMesh * mesh = pScene->mMeshes[i];

		model->name = std::string(mesh->mName.C_Str());

		model->materialIndex = mesh->mMaterialIndex;

		model->vertices.reserve(mesh->mNumVertices);
		model->indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			mesh->mVertices; mesh->mNormals; mesh->mTextureCoords;
			if (mesh->HasTextureCoords(0))
				model->hasTexture = true;
			if (mesh->HasNormals())
				model->hasNormal = true;
			
			SVertex vertex;
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;
			
			if (model->hasTexture)
			{
				vertex.TexC.x = mesh->mTextureCoords[0][i].x;
				vertex.TexC.y = mesh->mTextureCoords[0][i].y;
			}

			if (model->hasNormal)
			{
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}

			model->vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			assert(mesh->mFaces->mNumIndices == 3);
			model->indices.push_back(mesh->mFaces[i].mIndices[0]);
			model->indices.push_back(mesh->mFaces[i].mIndices[1]);
			model->indices.push_back(mesh->mFaces[i].mIndices[2]);
		}

		if (model->hasNormal && model->hasTexture)
		{
			model->hasTangent = true;
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				auto index0 = mesh->mFaces->mIndices[0];
				auto index1 = mesh->mFaces->mIndices[1];
				auto index2 = mesh->mFaces->mIndices[2];

				DirectX::XMFLOAT3 tangent, binormal;

				CalculateTangentAndBinormal(model->vertices[index0].Position,
					model->vertices[index1].Position, model->vertices[index2].Position,
					model->vertices[index0].TexC, model->vertices[index1].TexC,
					model->vertices[index2].TexC, tangent, binormal);

				model->vertices[index0].TangentU = tangent;
				model->vertices[index0].Binormal = binormal;

				model->vertices[index1].TangentU = tangent;
				model->vertices[index1].Binormal = binormal;

				model->vertices[index2].TangentU = tangent;
				model->vertices[index2].Binormal = binormal;
			}
		}
	}
}

void WriteModels(const std::vector<SModel>& models, const aiScene * pScene, const std::string& to)
{
	using namespace std;
	ofstream file((to + ".obl").c_str());
	streambuf * coutbuf = std::cout.rdbuf();
	cout.rdbuf(file.rdbuf());

	if (!pScene->HasAnimations())
		cout << "Static\n";
	else
		cout << "Animation\n";

	cout << "Meshes " << pScene->mNumMeshes << " { " << std::endl;

	for (auto & model : models)
	{
		cout << "\t" << model.name << " {" << std::endl;
		cout << "\t\tVertices " << model.vertices.size() << " ";
		if (model.hasTexture)
		{
			cout << "UV 1 ";
		}
		else
		{
			cout << "UV 0 ";
		}

		if (model.hasNormal)
		{
			cout << "Normals 1 ";
		}
		else
		{
			cout << "Normals 0 ";
		}

		if (model.hasTangent)
		{
			cout << "Others 1 ";
		}
		else
		{
			cout << "Others 0 ";
		}

		cout << "{\n";

		for (unsigned int i = 0; i < model.vertices.size(); ++i)
		{
			cout << "\t\t\t" << model.vertices[i].Position.x << ' ' << model.vertices[i].Position.y << ' ' << model.vertices[i].Position.z << ' ';
			if (model.hasTexture)
				cout << model.vertices[i].TexC.x << ' ' << model.vertices[i].TexC.y << ' ';
			if (model.hasNormal)
				cout << model.vertices[i].Normal.x << ' ' << model.vertices[i].Normal.y << ' ' << model.vertices[i].Normal.z << ' ';
			if (model.hasTangent)
			{
				cout << model.vertices[i].TangentU.x << ' ' << model.vertices[i].TangentU.y << ' ' << model.vertices[i].TangentU.z << ' ';
				cout << model.vertices[i].Binormal.x << ' ' << model.vertices[i].Binormal.y << ' ' << model.vertices[i].Binormal.z << ' ';
			}
			cout << '\n';

		}

		cout << "\t\t}\n";

		cout << "\t\tIndices " << model.indices.size() << " {\n";

		for (unsigned int i = 0; i < model.indices.size() / 3; ++i)
		{
			cout << "\t\t\t" << model.indices[(i * 3) + 0] << ' '
				<< model.indices[(i * 3) + 1] << ' ' << model.indices[(i * 3) + 2] << '\n';
		}

		cout << "\t\t}\n";

		cout << "Material " << model.materialIndex << '\n';

		cout << "\t}\n";
	}

	cout << "}\n";
}

void ConvertFile(std::string const& from, std::string & to, unsigned int flags)
{
	Assimp::Importer importer;
	
	const aiScene * pScene = importer.ReadFile(from.c_str(), flags);
	if (!pScene)
		std::cout << "Can't convert that file";
	PrintFromScene(pScene, to);
	/*std::vector<SModel> models;
	CreateModels(pScene, models);

	WriteModels(models, pScene, to);

	std::string materialFile;
	materialFile = to + ".material";
	WriteMaterial(pScene, materialFile);*/
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

#elif defined ENVIRONMENT64


int main()
{

}

#endif