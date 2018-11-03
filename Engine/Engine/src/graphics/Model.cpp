#include "Model.h"
#include "Helpers/GeometryGenerator.h"
#include "Helpers/RenderHelper.h"
#include "../COM/COMManager.h"
#include "../imgui/imgui.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/algorithm/string.hpp"

uint32_t Model::m_advancedCheckMinimum = 50;

/*
* Appends path (except last directory) to relative
*/
static inline void AppendPathToRelative(std::string& relative, std::string const& path)
{
	size_t where = path.rfind('\\');
	if (relative[0] != '\\')
		relative.insert(0, 1, '\\');
	
	char toAppend[MAX_PATH];
	strncpy_s(toAppend, sizeof(toAppend), path.c_str(), where);

	relative.insert(0, toAppend);
}

static void ReadTillEnd(std::ifstream &fin)
{
	if (fin.eof())
		return;
	char c;
	while (true)
	{
		c = fin.get();
		if (c == '}')
			break;
	}
}

Model::Model()
{
	//ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_materialBuffer,
	//	D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
	//	sizeof(Shader::material_t), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}

Model::~Model()
{
	Destroy();
}

void Model::Update(float frameTime)
{
	IGameObject::Update(frameTime);
}


void Model::Destroy()
{
}

inline uint32_t Model::GetIndexCount(int subObject) const
{
	return m_meshes[subObject].m_indexRange.end - m_meshes[subObject].m_indexRange.begin;
}

inline uint32_t Model::GetVertexCount(int subObject) const
{
	return m_meshes[subObject].m_vertexRange.end - m_meshes[subObject].m_vertexRange.begin;
}

CommonTypes::RayHitInfo Model::PickObject(DirectX::FXMVECTOR & rayPos, DirectX::FXMVECTOR & rayDir) const
{
	float minDist = FLT_MAX;
	RayHitInfo ret;
	ret.dist = -1.0f;
	ret.instanceID = -1;
	for (auto & instanceID : m_drawnInstances)
	{
		DirectX::BoundingBox boundingBox = m_boundingBox;
		m_boundingBox.Transform(boundingBox, m_objectWorld[instanceID]);

		float dist;
		if (boundingBox.Intersects(rayPos, rayDir, dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				ret.dist = minDist;
				ret.instanceID = instanceID;
			}
		}
	}
	return ret;
}

void Model::ImGuiChangeMaterial()
{
	ImGui::Begin(GetName().c_str());

	for (auto & material : m_materials)
	{
		if (ImGui::Button(material.name.c_str()))
		{
			if (m_selectedMaterial == &material)
				m_selectedMaterial = nullptr;
			else
				m_selectedMaterial = &material;
		}
	}

	ImGui::End();

	if (m_selectedMaterial)
	{
		bool updateMaterial = false;
		ImGui::Begin(m_selectedMaterial->name.c_str());
		
		ImGui::SliderFloat("Opacity", &m_selectedMaterial->opacity, 0.01f, 1.0f);
		ImGui::SliderFloat("Specular", &m_selectedMaterial->specular, 1.0f, 100.0f);

		ImGui::SliderFloat("Minimum tesselation", &m_selectedMaterial->tessMin, 1.0f, 64.0f);
		if (m_selectedMaterial->tessMin > m_selectedMaterial->tessMax)
			m_selectedMaterial->tessMax = m_selectedMaterial->tessMin;
		ImGui::SliderFloat("Maximum tesselaton", &m_selectedMaterial->tessMax, m_selectedMaterial->tessMin, 64.0f);

		ImGui::SliderFloat("Tesselation scale", &m_selectedMaterial->tessScale, 0.01f, 2.0f);

		ImGui::ColorEdit4("Diffuse color ", &m_selectedMaterial->diffuseColor.x);

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Textures");

		ImGui::Checkbox("Has texture ", &m_selectedMaterial->hasTexture);
		ImGui::Checkbox("Has bump map ", &m_selectedMaterial->hasBumpMap);
		ImGui::Checkbox("Has specular map ", &m_selectedMaterial->hasSpecularMap);

		if (m_selectedMaterial->diffuseTexture.get())
		{
			std::string path = "Diffuse texture: " + m_selectedMaterial->diffuseTexture->GetPath();
			ImGui::Text(path.c_str());
		}
		else
		{
			ImGui::Text("Diffuse texture: None");
		}

		if (ImGui::Button("Select diffuse texture"))
		{
			if (auto ret = COM::GetOpenFileDialog(L"Select diffuse texture", L"Select"); ret)
			{
				try
				{
					Texture * tex = new Texture((LPSTR)ret.value().c_str());

					m_selectedMaterial->diffuseTexture.reset(tex);
				}
				catch (...)
				{
					DX::OutputVDebugString(L"Couldn't open file %s.\n", ret.value());
				}
			}
		}

		if (m_selectedMaterial->bumpMap.get())
		{
			std::string path = "Bump texture: " + m_selectedMaterial->bumpMap->GetPath();
			ImGui::Text(path.c_str());
		}
		else
		{
			ImGui::Text("Bump texture: None");
		}


		if (ImGui::Button("Select bump texture"))
		{
			if (auto ret = COM::GetOpenFileDialog(L"Select bump texture", L"Select"); ret)
			{
				try
				{
					Texture * tex = new Texture((LPSTR)ret.value().c_str());

					m_selectedMaterial->bumpMap.reset(tex);
				}
				catch (...)
				{
					DX::OutputVDebugString(L"Couldn't open file %s.\n", ret.value());
				}
			}
		}

		if (m_selectedMaterial->specularMap.get())
		{
			std::string path = "Specular texture: " + m_selectedMaterial->specularMap->GetPath();
			ImGui::Text(path.c_str());
		}
		else
		{
			ImGui::Text("Specular texture: None");
		}


		if (ImGui::Button("Select specular texture"))
		{
			if (auto ret = COM::GetOpenFileDialog(L"Select specular texture", L"Select"); ret)
			{
				try
				{
					Texture * tex = new Texture((LPSTR)ret.value().c_str());

					m_selectedMaterial->specularMap.reset(tex);
				}
				catch (...)
				{
					DX::OutputVDebugString(L"Couldn't open file %s.\n", ret.value());
				}
			}
		}

		if (ImGui::Button("Save"))
		{
			WriteMaterials();
		}

		ImGui::End();


	}
}

void Model::SetMaterial(Rendering::Material const& mat, int materialIndex)
{
	m_materials[materialIndex] = mat;
}

void Model::ReadMaterials(std::string const & filename)
{
	typedef boost::property_tree::iptree ptree;
	ptree materials;
	boost::property_tree::read_json(filename, materials);
	
	m_materialFile = filename;

	for (const std::pair<std::string, ptree>& material : materials)
	{
		Rendering::Material resultMaterial;
		resultMaterial.name = material.first;

		const ptree& materialTree = material.second;

		auto textureChild = materialTree.get_child_optional("texture");
		if (textureChild.is_initialized())
		{
			resultMaterial.hasTexture = true;
			std::string texturePath = textureChild.value().get_value<std::string>();
			resultMaterial.diffuseTexture = std::make_shared<Texture>((LPSTR)texturePath.c_str());
		}
		else
		{
			std::string values = materialTree.get_child("color").get_value<std::string>();
			std::stringstream stream(values);
			stream >> resultMaterial.diffuseColor.x >> resultMaterial.diffuseColor.y
				>> resultMaterial.diffuseColor.z >> resultMaterial.diffuseColor.w;
		}
		
		auto bumpChild = materialTree.get_child_optional("bump");
		if (bumpChild.is_initialized())
		{
			resultMaterial.hasBumpMap = true;
			std::string texturePath = textureChild.value().get_value<std::string>();
			resultMaterial.bumpMap = std::make_shared<Texture>((LPSTR)texturePath.c_str());
		}

		auto specularChild = materialTree.get_child_optional("specular map");
		if (specularChild.is_initialized())
		{
			resultMaterial.hasSpecularMap = true;
			std::string texturePath = textureChild.value().get_value<std::string>();
			resultMaterial.specularMap = std::make_shared<Texture>((LPSTR)texturePath.c_str());
		}

		resultMaterial.specular = materialTree.get_child("shininess").get_value<float>();
		resultMaterial.opacity = materialTree.get_child("opacity").get_value<float>();

		resultMaterial.tessMin = materialTree.get_child("tesselation.minimum").get_value<float>();
		resultMaterial.tessMax = materialTree.get_child("tesselation.maximum").get_value<float>();
		resultMaterial.tessScale = materialTree.get_child("tesselation.scale").get_value<float>();

		m_materials.push_back(std::move(resultMaterial));
	}

}

void Model::WriteMaterials()
{
	using boost::property_tree::ptree;
	ptree materialsTree;

	for (auto & material : m_materials)
	{
		ptree currentMaterialTree;

		if (material.hasTexture)
		{
			currentMaterialTree.put("Texture", material.diffuseTexture->GetPath());
		}
		else
		{
			std::stringstream color;
			color << material.diffuseColor.x << ' ' << material.diffuseColor.y << ' '
				<< material.diffuseColor.z << ' ' << material.diffuseColor.w;
			currentMaterialTree.put("Color", color.str());
		}

		if (material.hasBumpMap)
		{
			currentMaterialTree.put("Bump", material.bumpMap->GetPath());
		}

		if (material.hasSpecularMap)
		{
			currentMaterialTree.put("Specular map", material.specularMap->GetPath());
		}

		currentMaterialTree.put("Shininess", material.specular);
		currentMaterialTree.put("Opacity", material.opacity);
		currentMaterialTree.put("Tesselation.Minimum", material.tessMin);
		currentMaterialTree.put("Tesselation.Maximum", material.tessMax);
		currentMaterialTree.put("Tesselation.Scale", material.tessScale);

		materialsTree.put_child(material.name, currentMaterialTree);
	}

	boost::property_tree::write_json(m_materialFile, materialsTree);
}

void Model::DrawIndexedInstanced(ICamera * cam, const std::function<void(UINT, UINT, UINT)>& renderFunction) const
{
	//std::function<bool(uint32_t)> func = std::bind(&Model::ShouldRenderInstance, this, cam, std::placeholders::_1);
	m_drawnInstances.clear();
	std::function<bool(uint32_t) > f1 = [&](uint32_t instanceID)->bool const
	{
		if (ShouldRenderInstance(cam, instanceID))
		{
			m_drawnInstances.push_back(instanceID);
			return true;
		}
		else
		{
			return false;
		}
		
	};
	std::function<bool(const Model::Mesh&, uint32_t)> shouldRenderMesh;
	if (m_meshes.size() > m_advancedCheckMinimum)
	{
		shouldRenderMesh = [&](const Model::Mesh& m, uint32_t instanceID)->bool const
		{
			auto frustum = cam->GetFrustum();
			DirectX::BoundingBox toRender;
			m.m_boundingBox.Transform(toRender, m_objectWorld[instanceID]);
			return frustum.Contains(toRender);
		};
	
	}
	else
	{
		shouldRenderMesh = [&](const Model::Mesh& m, uint32_t instanceID)->bool const
		{
			return true;
		};
	}
	uint32_t renderInstances = PrepareInstances(f1);
	ID3D11Buffer * instances[] =
	{
		m_instanceBuffer.Get()
	};
	UINT stride = sizeof(DirectX::XMMATRIX);
	UINT offset = 0;
	m_d3d11Context->IASetVertexBuffers(1, 1, instances, &stride, &offset);
	if (renderInstances == 0)
		return;
	
	/// TODO: Fix this. Make it render a mesh when it's visible at least one
	//for (uint32_t i = 0; i < m_drawnInstances.size(); ++i) /// Might be useful another time
	{
		for (auto & mesh : m_meshes)
		{
			if (m_materials[mesh.m_materialIndex].opacity != 1.0f)
				continue;
			//if (shouldRenderMesh(mesh, m_drawnInstances[i]))
			{
				BindMaterial(m_materials[mesh.m_materialIndex], m_bindMaterialToShader);
				//m_d3d11Context->DrawIndexedInstanced((UINT)(mesh.m_indexRange.end - mesh.m_indexRange.begin),
				//	(UINT)renderInstances, (UINT)mesh.m_indexRange.begin,
				//	(UINT)mesh.m_vertexRange.begin, 0);
				renderFunction((UINT)mesh.m_indexRange.end - mesh.m_indexRange.begin,
					(UINT)mesh.m_indexRange.begin, (UINT)mesh.m_vertexRange.begin);
				
				if (g_isDeveloper)
					g_drawCalls++;
			}
		}
	}
	auto renderer = Direct3D11::Get();
	//for (uint32_t i = 0; i < m_drawnInstances.size(); ++i) /// Might be useful another time
	{
		for (auto & mesh : m_meshes)
		{
			float opacity = m_materials[mesh.m_materialIndex].opacity;
			if (opacity == 1.0f)
				continue;
			//if (shouldRenderMesh(mesh, m_drawnInstances[i]))
			{
				renderer->OMTransparency(opacity);
				BindMaterial(m_materials[mesh.m_materialIndex], m_bindMaterialToShader);
				//m_d3d11Context->DrawIndexedInstanced((UINT)(mesh.m_indexRange.end - mesh.m_indexRange.begin),
				//	(UINT)renderInstances, (UINT)mesh.m_indexRange.begin,
				//	(UINT)mesh.m_vertexRange.begin, 0);
				renderFunction((UINT)mesh.m_indexRange.end - mesh.m_indexRange.begin,
					(UINT)mesh.m_indexRange.begin, (UINT)mesh.m_vertexRange.begin);
				if (g_isDeveloper)
				{
					g_drawCalls++;
				}
			}
		}
	}
	renderer->OMDefaultBlend();


	if (g_isDeveloper)
	{
		auto GraphicsDebugDrawer = GraphicsDebugDraw::Get();
		if (GraphicsDebugDrawer->GetDebugFlags() & DBG_DRAW_BOUNDING_BOX)
		{
			DirectX::BoundingBox toRender;
			for (uint32_t i = 0; i < m_objectWorld.size(); ++i)
			{
				m_boundingBox.Transform(toRender, m_objectWorld[i]);
				GraphicsDebugDrawer->RenderBoundingBox(toRender);
			}
			/*for (uint32_t i = 0; i < m_objectWorld.size(); ++i)
			{
				for (uint32_t j = 0; j < m_meshes.size(); ++j)
				{
					m_meshes[j].m_boundingBox.Transform(toRender, m_objectWorld[i]);
					GraphicsDebugDrawer->RenderBoundingBox(toRender);
				}
			}*/
		}
	}

}

bool Model::PrepareIA(const PipelineEnum & p) const
{
	if (p == PipelineEnum::PipelineDisplacementTextureLight)
	{
		m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	}
	else
	{
		m_d3d11Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	return true;
}

bool Model::ShouldRenderInstance(ICamera * cam, uint32_t id) const
{
	auto frustum = cam->GetFrustum();

	DirectX::BoundingBox toRender;

	toRender = m_boundingBox;
	m_boundingBox.Transform(toRender, m_objectWorld[id]);

	if (frustum.Contains(toRender))
		return true;

	return false;
}

void Model::Create(std::string const& filename)
{
	using boost::algorithm::to_lower_copy;
	if (to_lower_copy(filename) == "cube")
	{
		this->Create(EDefaultObject::Box);
		return;
	}
	if (to_lower_copy(filename) == "grid")
	{
		this->Create(EDefaultObject::Grid);
		return;
	}
	if (to_lower_copy(filename) == "sphere")
	{
		this->Create(EDefaultObject::Sphere);
		return;
	}
	if (to_lower_copy(filename) == "cylinder")
	{
		this->Create(EDefaultObject::Cylinder);
		return;
	}
	std::ifstream fin;

	fin.open((filename + ".obl").c_str());
	
	if (!fin.is_open())
		THROW_ERROR("Can't find file %s", (filename + ".obl").c_str());

#define INVALID THROW_ERROR("Model %s is invalid.",filename.c_str())

	std::string check;
	int checkInt;
	fin >> check;
	if (check != "Static")
		THROW_ERROR("Can't use model %s as a model because it is not static", filename.c_str());

	fin >> check;
	if (check != "Meshes")
		THROW_ERROR("Model %s is invalid due to it not having \"Meshes\"", filename.c_str());

	int meshCount;
	fin >> meshCount;
	std::getline(fin, check);
	m_meshes.emplace_back();
	m_meshes.resize(meshCount);

	DirectX::XMFLOAT3 globalMin(FLT_MAX, FLT_MAX, FLT_MAX);
	DirectX::XMFLOAT3 globalMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	auto checkAndSwapSmallest = [](float& a, float b)
	{
		if (b < a)
			a = b;
	};
	auto checkAndSwapLargest = [](float& a, float b)
	{
		if (b > a)
			a = b;
	};

	std::vector<Oblivion::SVertex> vertices;

	for (int i = 0; i < meshCount; ++i)
	{
		std::string meshName;
		fin >> meshName;
		std::getline(fin, check);
		
#pragma region Read vertices
		fin >> check;
		if (check != "Vertices")
			THROW_ERROR("Model %s is invalid due to it not having \"Vertices\" in mesh %d", filename.c_str(), i);

		int numVertices;
		int startVertices = (int)vertices.size();
		fin >> numVertices;
		vertices.reserve(vertices.size() + numVertices);

		fin >> check;
		if (check != "UV")
			THROW_ERROR("Model %s is invalid due to it not having \"UV\" in mesh %d", filename.c_str(), i);
		bool hasTexture, hasNormals, hasOther;
		fin >> checkInt;
		hasTexture = checkInt ? true : false;
		
		fin >> check;
		if (check != "Normals")
			THROW_ERROR("Model %s is invalid due to it not having \"Normals\" in mesh %d", filename.c_str(), i);
		fin >> checkInt;
		hasNormals = checkInt ? true : false;

		fin >> check;
		if (check != "Others")
			THROW_ERROR("Model %s is invalid due to it not having \"Others\" in mesh %d", filename.c_str(), i);
		fin >> checkInt;
		hasOther = checkInt ? true : false;
		
		getline(fin, check);

		// Kinda useless
		DirectX::XMFLOAT3 localMin(FLT_MAX, FLT_MAX, FLT_MAX);
		DirectX::XMFLOAT3 localMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int j = 0; j < numVertices; ++j)
		{
			vertices.emplace_back();
			float x, y, z;
			fin >> x >> y >> z;
			vertices.back().Position = { x,y,z };
			checkAndSwapSmallest(globalMin.x, x); checkAndSwapLargest(globalMax.x, x);
			checkAndSwapSmallest(globalMin.y, y); checkAndSwapLargest(globalMax.y, y);
			checkAndSwapSmallest(globalMin.z, z); checkAndSwapLargest(globalMax.z, z);

			// Kinda useless
			checkAndSwapSmallest(localMin.x, x); checkAndSwapLargest(localMax.x, x);
			checkAndSwapSmallest(localMin.y, y); checkAndSwapLargest(localMax.y, y);
			checkAndSwapSmallest(localMin.z, z); checkAndSwapLargest(localMax.z, z);
			
			if (hasTexture)
			{
				fin >> x >> y;
				vertices.back().TexC = { x,y, 0.0f, 0.0f };
			}
			if (hasNormals)
			{
				fin >> x >> y >> z;
				assert(!(x == 0 && y == 0 && z == 0));
				vertices.back().Normal = { x,y,z };
			}
			if (hasOther)
			{ 
				fin >> x >> y >> z;
				//assert(!(x == 0 && y == 0 && z == 0));
				if (x == 0 && y == 0 && z == 0)
					x = 1.0f, y = 0.0f, z = 0.0f;
				vertices.back().TangentU = { x,y,z };
				fin >> x >> y >> z;
				//assert(!(x == 0 && y == 0 && z == 0));
				if (x == 0 && y == 0 && z == 0)
					x = 0.0f, y = 0.0f, z = 1.0f;
				vertices.back().Binormal = { x,y,z };
			}
		}
		ReadTillEnd(fin);

		m_meshes[i].m_vertexRange = AddVertices(vertices, startVertices, (int)vertices.size());

		// Kinda useless
		DirectX::XMFLOAT3 center, offset;
		center = DirectX::XMFLOAT3(
			(localMax.x + localMin.x) / 2.f,
			(localMax.y + localMin.y) / 2.f,
			(localMax.z + localMin.z) / 2.f
		);
		offset = DirectX::XMFLOAT3(
			localMax.x - center.x,
			localMax.y - center.y,
			localMax.z - center.z
		);
		m_meshes[i].m_boundingBox = DirectX::BoundingBox(
			center, offset
		);

#pragma endregion

#pragma region Read Indices and Material
		fin >> check;
		if (check != "Indices")
			THROW_ERROR("Model %s is invalid due to it not having \"Indices\" in mesh %d", filename.c_str(), i);
		int numIndices;
		fin >> numIndices;
		std::getline(fin, check);

		//int startIndices = (int)m_indices.size();
		//m_indices.reserve(m_indices.size() + numIndices);
		std::vector<uint32_t> indices;
		indices.reserve(numIndices);
		for (int i = 0; i < numIndices; ++i)
		{
			int index;
			fin >> index;
			indices.push_back(index);
		}

		m_meshes[i].m_indexRange = AddIndices(indices);


		ReadTillEnd(fin);

		fin >> check;
		if (check != "Material")
			THROW_ERROR("Model %s is invalid due to it not having \"Material\" in mesh %d", filename.c_str(), i);

		fin >> m_meshes[i].m_materialIndex;

		ReadTillEnd(fin);
#pragma endregion

	}

	ReadTillEnd(fin);

	ReadMaterials(filename + ".material");

#undef INVALID

	DirectX::XMFLOAT3 center, offset;
	center = DirectX::XMFLOAT3(
		(globalMax.x + globalMin.x) / 2.f,
		(globalMax.y + globalMin.y) / 2.f,
		(globalMax.z + globalMin.z) / 2.f
	);
	offset = DirectX::XMFLOAT3(
		globalMax.x - center.x,
		globalMax.y - center.y,
		globalMax.z - center.z
	);
	m_boundingBox = DirectX::BoundingBox(
		center, offset
	);

	fin.close();
}

void Model::Create(EDefaultObject object)
{
	GeometryGenerator g;
	GeometryGenerator::MeshData data;
	if (object == EDefaultObject::Box)
	{
		g.CreateBox(2.0f, 2.0f, 2.0f, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	else if (object == EDefaultObject::Cylinder)
	{
		g.CreateCylinder(0.5f, 0.5f, 3.0f, 32, 32, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 3.0f, 1.0f));
	}
	else if (object == EDefaultObject::Geosphere)
	{
		g.CreateGeosphere(1.0f, 1, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	else if (object == EDefaultObject::Grid)
	{
		g.CreateGrid(100.0f, 100.0f, 3, 3, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(50.0f, 0.01f, 50.0f));
	}
	else if (object == EDefaultObject::Sphere)
	{
		g.CreateSphere(1.0f, 12, 12, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	std::vector<Oblivion::SVertex> vertices;
	std::vector<uint32_t> indices;
	vertices = std::move(data.Vertices);
	indices = std::move(data.Indices);
	m_meshes.emplace_back();
	try
	{
		if (object != EDefaultObject::Grid)
		{
			m_materials.emplace_back();
			m_materials.back().hasBumpMap = TRUE;
			m_materials.back().hasSpecularMap = FALSE;
			m_materials.back().hasTexture = TRUE;
			m_materials.back().name = "Default";
			m_materials.back().opacity = 1.0f;
			m_materials.back().tessMin = 1.0f;
			m_materials.back().tessMax = 3.0f;
			m_materials.back().tessScale = 0.1f;
			m_materials.back().specular = 1000.0f;
			m_materials.back().diffuseTexture = std::make_shared<Texture>((LPWSTR)L"Resources/Stones.dds");
			m_materials.back().bumpMap = std::make_shared<Texture>((LPWSTR)L"Resources/Stones_nmap.dds");
			m_meshes.back().m_materialIndex = 0;
		}
		else
		{
			m_materials.emplace_back();
			m_materials.back().hasBumpMap = TRUE;
			m_materials.back().hasSpecularMap = FALSE;
			m_materials.back().hasTexture = TRUE;
			m_materials.back().name = "Default";
			m_materials.back().opacity = 1.0f;
			m_materials.back().specular = 1000.0f;
			m_materials.back().tessMin = 1.0f;
			m_materials.back().tessMax = 64.0f;
			m_materials.back().tessScale = 0.3f;
			m_materials.back().diffuseTexture = std::make_shared<Texture>((LPWSTR)L"Resources/Stones.dds");
			m_materials.back().bumpMap = std::make_shared<Texture>((LPWSTR)L"Resources/Stones_nmap.dds");
			m_meshes.back().m_materialIndex = 0;
		}
	}
	catch (...)
	{
		THROW_ERROR("Can't open texture: \"Resources/stones.jpg\"");
	}

	m_meshes.back().m_vertexRange = AddVertices(vertices);
	m_meshes.back().m_indexRange = AddIndices(indices);
}