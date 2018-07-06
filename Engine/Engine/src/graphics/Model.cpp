#include "Model.h"
#include "Helpers/GeometryGenerator.h"


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

Model::Model()
{
	ShaderHelper::CreateBuffer(m_d3d11Device.Get(), &m_materialBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(Shader::material_t), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
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
	return uint32_t(m_startIndices[subObject].end - m_startIndices[subObject].begin);
}

inline uint32_t Model::GetVertexCount() const
{
	return (uint32_t)m_vertices.size();
}

bool Model::ShouldRenderInstance() const
{
	return true;
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

void Model::DrawIndexedInstanced() const
{
	auto data = (DirectX::XMMATRIX*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_instanceBuffer.Get());
	uint32_t renderInstances = 0;
	for (size_t i = 0; i < m_objectWorld.size(); ++i)
	{
		if (ShouldRenderInstance())
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
	for (size_t i = 0; i < m_startIndices.size(); ++i)
	{
		if (m_materials[m_materialIndices[i]].opacity != 1.0f)
			continue;
		BindMaterial(m_materialIndices[i], (int)Shader::ShaderType::ePixel);
		m_d3d11Context->DrawIndexedInstanced((UINT)GetIndexCount((int)i),
			(UINT)renderInstances, (UINT)m_startIndices[i].begin,
			(UINT)m_verticesRange[i].begin, 0);
	}
	auto renderer = Direct3D11::Get();
	for (size_t i = 0; i < m_startIndices.size(); ++i)
	{
		float opacity = m_materials[m_materialIndices[i]].opacity;
		if (opacity == 1.0f)
			continue;
		renderer->OMTransparency(opacity);
		BindMaterial(m_materialIndices[i], (int)Shader::ShaderType::ePixel);
		m_d3d11Context->DrawIndexedInstanced((UINT)GetIndexCount((int)i),
			(UINT)renderInstances, (UINT)m_startIndices[i].begin,
			(UINT)m_verticesRange[i].begin, 0);
	}
	renderer->OMDefault();
}

void Model::BindMaterial(int index, int shader) const
{
	assert((size_t)index < m_materials.size());
	
	auto data = (Shader::material_t*)ShaderHelper::MapBuffer(m_d3d11Context.Get(), m_materialBuffer.Get());

	data->color			= m_materials[index].diffuseColor;
	data->hasTexture	= m_materials[index].hasTexture;
	data->hasBump		= m_materials[index].hasBumpMap;
	data->hasSpecular	= m_materials[index].hasSpecularMap;
	data->specular		= m_materials[index].specular;
	data->color			= m_materials[index].diffuseColor;

	ShaderHelper::UnmapBuffer(m_d3d11Context.Get(), m_materialBuffer.Get());

	ID3D11ShaderResourceView *resources[] = 
	{
		m_materials[index].diffuseTexture ? m_materials[index].diffuseTexture->GetTextureSRV() : nullptr,
		m_materials[index].bumpMap ? m_materials[index].bumpMap->GetTextureSRV() : nullptr,
		m_materials[index].specularMap ? m_materials[index].specularMap->GetTextureSRV() : nullptr,
	};
	

	if (shader & (int)Shader::ShaderType::eVertex)
	{
		m_d3d11Context->VSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->VSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eHull)
	{
		m_d3d11Context->HSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->HSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eDomain)
	{
		m_d3d11Context->DSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->DSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::eGeometry)
	{
		m_d3d11Context->GSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->GSSetShaderResources(0, 3, resources);
	}
	if (shader & (int)Shader::ShaderType::ePixel)
	{
		m_d3d11Context->PSSetConstantBuffers(2, 1, m_materialBuffer.GetAddressOf());
		m_d3d11Context->PSSetShaderResources(0, 3, resources);
	}
}

void ReadTillEnd(std::ifstream &fin)
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
	m_materialIndices.resize(meshCount);

	m_vertices;

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
		int startVertices = (int)m_vertices.size();
		fin >> numVertices;
		m_vertices.reserve(m_vertices.size() + numVertices);

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

		for (int j = 0; j < numVertices; ++j)
		{
			m_vertices.emplace_back();
			float x, y, z;
			fin >> x >> y >> z;
			m_vertices.back().Position = { x,y,z };
			if (hasTexture)
			{
				fin >> x >> y;
				m_vertices.back().TexC = { x,y };
			}
			if (hasNormals)
			{
				fin >> x >> y >> z;
				m_vertices.back().Normal = { x,y,z };
			}
			if (hasOther)
			{
				fin >> x >> y >> z;
				m_vertices.back().TangentU = { x,y,z };
				fin >> x >> y >> z;
				m_vertices.back().Binormal = { x,y,z };
			}
		}
		ReadTillEnd(fin);

		m_verticesRange.push_back(AddVertices(m_vertices, startVertices, (int)m_vertices.size()));

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

		m_startIndices.emplace_back(startIndices, (uint32_t)m_indices.size());


		ReadTillEnd(fin);

		fin >> check;
		if (check != "Material")
			THROW_ERROR("Model %s is invalid due to it not having \"Material\" in mesh %d", filename.c_str(), i);

		fin >> m_materialIndices[i];

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

	fin.close();
}

void Model::Create(EDefaultObject object)
{
	GeometryGenerator g;
	GeometryGenerator::MeshData data;
	if (object == EDefaultObject::Box)
		g.CreateBox(1.0f, 1.0f, 1.0f, data);
	else if (object == EDefaultObject::Cylinder)
		g.CreateCylinder(1.0f, 1.0f, 3.0f, 32, 32, data);
	else if (object == EDefaultObject::FullscreenQuad)
		g.CreateFullscreenQuad(data);
	else if (object == EDefaultObject::Geosphere)
		g.CreateGeosphere(1.0f, 12, data);
	else if (object == EDefaultObject::Grid)
		g.CreateGrid(100.0f, 100.0f, 20, 20, data);
	else if (object == EDefaultObject::Sphere)
		g.CreateSphere(1.0f, 20, 20, data);
	m_vertices = std::move(data.Vertices);
	m_indices = std::move(data.Indices);
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
			m_materialIndices.push_back(0);
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
			m_materialIndices.push_back(0);
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

	m_verticesRange.push_back(AddVertices(m_vertices));
	m_startIndices.emplace_back(0, (uint32_t)m_indices.size());
}