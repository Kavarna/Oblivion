#include "Model.h"
#include "Helpers/GeometryGenerator.h"
#include "Helpers/RenderHelper.h"

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


void Model::Destroy()
{
	m_indexBuffer.Reset();
	m_materialBuffer.Reset();
}

inline uint32_t Model::GetIndexCount(int subObject) const
{
	return m_meshes[subObject].m_indexRange.end - m_meshes[subObject].m_indexRange.begin;
}

inline uint32_t Model::GetVertexCount(int subObject) const
{
	return m_meshes[subObject].m_vertexRange.end - m_meshes[subObject].m_vertexRange.begin;
}

void Model::SetMaterial(Rendering::Material && mat, int materialIndex)
{
	m_materials[materialIndex] = std::move(mat);
}

void Model::RenderBasicShader(ICamera* cam) const
{
	static BasicShader * shader = BasicShader::Get();
	static DirectX::XMMATRIX VP;
	VP = cam->GetView() * cam->GetProjection();
	VP = DirectX::XMMatrixTranspose(VP);
	shader->SetCameraInformations({ VP });
}

void Model::RenderTextureLightShader(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureLightShader * shader = TextureLightShader::Get();
	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(cam->GetView()),
		DirectX::XMMatrixTranspose(cam->GetProjection())
		});

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSamplerState.GetAddressOf());
}

void Model::RenderTexture(ICamera * cam) const
{
	static auto renderer = Direct3D11::Get();
	static TextureShader * shader = TextureShader::Get();
	shader->SetCameraInformations({
		DirectX::XMMatrixTranspose(cam->GetView()),
		DirectX::XMMatrixTranspose(cam->GetProjection())
		});

	m_d3d11Context->PSSetSamplers(0, 1, renderer->m_linearWrapSamplerState.GetAddressOf());
}

void Model::DrawIndexedInstanced(ICamera * cam) const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	for (size_t i = 0; i < m_objectWorld.size(); ++i)
	{
		if (ShouldRenderInstance(cam, (uint32_t)i))
		{
			data[renderInstances++] = m_objectWorld[i];
		}
	}
	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	ID3D11Buffer * instances[] =
	{
		m_instanceBuffer.Get()
	};
	UINT stride = sizeof(DirectX::XMMATRIX);
	UINT offset = 0;
	m_d3d11Context->IASetVertexBuffers(1, 1, instances, &stride, &offset);
	if (renderInstances == 0)
		return;
	
	for (auto & mesh : m_meshes)
	{
		if (m_materials[mesh.m_materialIndex].opacity != 1.0f)
			continue;
		BindMaterial(m_materials[mesh.m_materialIndex], (int)Shader::ShaderType::ePixel);
		m_d3d11Context->DrawIndexedInstanced((UINT)(mesh.m_indexRange.end - mesh.m_indexRange.begin),
			(UINT)renderInstances, (UINT)mesh.m_indexRange.begin,
			(UINT)mesh.m_vertexRange.begin, 0);
#if DEBUG || _DEBUG
		g_drawCalls++;
#endif
	}
	auto renderer = Direct3D11::Get();
	for (auto & mesh : m_meshes)
	{
		float opacity = m_materials[mesh.m_materialIndex].opacity;
		if (opacity == 1.0f)
			continue;
		renderer->OMTransparency(opacity);
		BindMaterial(m_materials[mesh.m_materialIndex], (int)Shader::ShaderType::ePixel);
		m_d3d11Context->DrawIndexedInstanced((UINT)(mesh.m_indexRange.end - mesh.m_indexRange.begin),
			(UINT)renderInstances, (UINT)mesh.m_indexRange.begin,
			(UINT)mesh.m_vertexRange.begin, 0);
#if DEBUG || _DEBUG
		g_drawCalls++;
#endif
	}
	renderer->OMDefault();

#if defined DRAW_AABB

	DirectX::BoundingBox toRender;
	for (uint32_t i = 0; i < m_objectWorld.size(); ++i)
	{
		toRender = m_boundingBox;
		toRender.Transform(toRender, m_objectWorld[i]);
		RenderHelper::RenderBoundingBox(cam, toRender);
	}

#endif
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
	std::ifstream fin;

	fin.open(filename.c_str());
	
	if (!fin.is_open())
		THROW_ERROR("Can't find file %s", filename.c_str());

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
		/*DirectX::XMFLOAT3 localMin(FLT_MAX, FLT_MAX, FLT_MAX);
		DirectX::XMFLOAT3 localMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);*/

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
			/*checkAndSwapSmallest(localMin.x, x); checkAndSwapLargest(localMax.x, x);
			checkAndSwapSmallest(localMin.y, y); checkAndSwapLargest(localMax.y, y);
			checkAndSwapSmallest(localMin.z, z); checkAndSwapLargest(localMax.z, z);*/
			
			if (hasTexture)
			{
				fin >> x >> y;
				vertices.back().TexC = { x,y };
			}
			if (hasNormals)
			{
				fin >> x >> y >> z;
				vertices.back().Normal = { x,y,z };
			}
			if (hasOther)
			{
				fin >> x >> y >> z;
				vertices.back().TangentU = { x,y,z };
				fin >> x >> y >> z;
				vertices.back().Binormal = { x,y,z };
			}
		}
		ReadTillEnd(fin);

		m_meshes[i].m_vertexRange = AddVertices(vertices, startVertices, (int)vertices.size());

		// Kinda useless
		/*DirectX::XMFLOAT3 center, offset;
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
		);*/

#pragma endregion

#pragma region Read Indices and Material
		fin >> check;
		if (check != "Indices")
			THROW_ERROR("Model %s is invalid due to it not having \"Indices\" in mesh %d", filename.c_str(), i);
		int numIndices;
		fin >> numIndices;
		std::getline(fin, check);

		int startIndices = (int)m_indices.size();
		m_indices.reserve(m_indices.size() + numIndices);
		for (int i = 0; i < numIndices; ++i)
		{
			int index;
			fin >> index;
			m_indices.push_back(index);
		}

		m_meshes[i].m_indexRange = { (uint32_t)startIndices, (uint32_t)m_indices.size() };


		ReadTillEnd(fin);

		fin >> check;
		if (check != "Material")
			THROW_ERROR("Model %s is invalid due to it not having \"Material\" in mesh %d", filename.c_str(), i);

		fin >> m_meshes[i].m_materialIndex;

		ReadTillEnd(fin);
#pragma endregion

	}

	ReadTillEnd(fin);

	fin >> check;
	if (check != "Materials")
		THROW_ERROR("Model %s is invalid due to it not having \"Materials\"", filename.c_str());

	int materialCount;
	fin >> materialCount;

	m_materials.resize(materialCount);
	std::getline(fin, check);

#pragma region Read Materials
	for (int i = 0; i < materialCount; ++i)
	{
		std::string materialName;
		fin >> m_materials[i].name;
		std::getline(fin, check);

		while (true)
		{
			fin >> check;
			if (check == "Texture")
			{
				std::string path;
				std::getline(fin, path);
				if (path[0] == ' ')
					path.erase(0, 1);
				AppendPathToRelative(path, filename);
				m_materials[i].diffuseTexture = std::make_unique<Texture>(
					(LPSTR)path.c_str(), m_d3d11Device.Get(), m_d3d11Context.Get()
					);
				m_materials[i].hasTexture = true;
			}
			else if (check == "Color")
			{
				float r, g, b, a;
				fin >> r >> g >> b >> a;
				m_materials[i].diffuseColor = { r,g,b,a };
			}
			else if (check == "Bump")
			{
				std::string path;
				std::getline(fin, path);
				if (path[0] == ' ')
					path.erase(0, 1);
				AppendPathToRelative(path, filename);
				m_materials[i].bumpMap = std::make_unique<Texture>(
					(LPSTR)path.c_str(), m_d3d11Device.Get(), m_d3d11Context.Get()
					);
				m_materials[i].hasBumpMap = true;
			}
			else if (check == "Shininess")
			{
				float val;
				fin >> val;
				m_materials[i].specular = val;
			}
			else if (check == "Specular")
			{
				std::string path;
				std::getline(fin, path);
				if (path[0] == ' ')
					path.erase(0, 1);
				AppendPathToRelative(path, filename);
				m_materials[i].specularMap = std::make_unique<Texture>(
					(LPSTR)path.c_str(), m_d3d11Device.Get(), m_d3d11Context.Get()
					);
				m_materials[i].hasSpecularMap = true;
			}
			else if (check == "Opacity")
			{
				float val;
				fin >> val;
				m_materials[i].opacity = val;
			}
			else if (check == "}")
				break;
			else
				THROW_ERROR("Model %s is invalid due to it having %s, which is not a known property", filename.c_str(), check.c_str());
		}

	}

	ReadTillEnd(fin);

#pragma endregion

#undef INVALID

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);

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
		g.CreateBox(1.0f, 1.0f, 1.0f, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	else if (object == EDefaultObject::Cylinder)
	{
		g.CreateCylinder(1.0f, 1.0f, 3.0f, 32, 32, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 3.0f, 1.0f));
	}
	else if (object == EDefaultObject::Geosphere)
	{
		g.CreateGeosphere(1.0f, 1, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	else if (object == EDefaultObject::Grid)
	{
		g.CreateGrid(100.0f, 100.0f, 20, 20, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(50.0f, 0.01f, 50.0f));
	}
	else if (object == EDefaultObject::Sphere)
	{
		g.CreateSphere(1.0f, 20, 20, data);
		m_boundingBox = DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	}
	std::vector<Oblivion::SVertex> vertices;
	vertices = std::move(data.Vertices);
	m_indices = std::move(data.Indices);
	m_meshes.emplace_back();
	try
	{
		if (object != EDefaultObject::Grid)
		{
			m_materials.emplace_back();
			m_materials.back().hasBumpMap = FALSE;
			m_materials.back().hasSpecularMap = FALSE;
			m_materials.back().hasTexture = TRUE;
			m_materials.back().name = "Default";
			m_materials.back().opacity = 1.0f;
			m_materials.back().specular = 1000.0f;
			m_materials.back().diffuseTexture = std::make_unique<Texture>((LPWSTR)L"Resources/Marble.jpg", m_d3d11Device.Get(), m_d3d11Context.Get());
			m_meshes.back().m_materialIndex = 0;
		}
		else
		{
			m_materials.emplace_back();
			m_materials.back().hasBumpMap = FALSE;
			m_materials.back().hasSpecularMap = FALSE;
			m_materials.back().hasTexture = TRUE;
			m_materials.back().name = "Default";
			m_materials.back().opacity = 1.0f;
			m_materials.back().specular = 1000.0f;
			m_materials.back().diffuseTexture = std::make_unique<Texture>((LPWSTR)L"Resources/Grass.jpg", m_d3d11Device.Get(), m_d3d11Context.Get());
			m_meshes.back().m_materialIndex = 0;
		}
	}
	catch (...)
	{
		DX::OutputVDebugString(L"Can't open texture: \"Resources/Marble.jpg\"\n");
		throw std::exception("Can't find resources");
	}

	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_indexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof(decltype(m_indices[0])) * m_indices.size(), 0, &m_indices[0]);

	m_meshes.back().m_vertexRange = AddVertices(vertices);
	m_meshes.back().m_indexRange = { 0, (uint32_t)m_indices.size() };
}