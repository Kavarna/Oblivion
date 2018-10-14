#include "BillboardObject.h"

BillboardObject::BillboardObject()
{
}

BillboardObject::~BillboardObject()
{
}

void BillboardObject::CreateFromTexture(std::shared_ptr<Texture> texture)
{
	Square::Create();
	this->SetTexture(texture);
}

void BillboardObject::Destroy()
{
}

void BillboardObject::DrawIndexedInstanced(ICamera * cam) const
{
	auto renderer = Direct3D11::Get();
	std::function<bool(uint32_t)> func = [&](uint32_t index) -> bool
	{
		static DirectX::BoundingBox bb(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f));
		DirectX::BoundingFrustum frustum = cam->GetFrustum();
		DirectX::BoundingBox transformedBoundingBox;
		bb.Transform(transformedBoundingBox, m_objectWorld[index]);
		return frustum.Contains(transformedBoundingBox);
	};
	int renderInstances = PrepareInstances(func, [&](DirectX::FXMMATRIX mat)
	{
		static float angleS = 0.0f;
		angleS += 0.01f;
		auto camPos = cam->GetPosition();
		mat; // Should not have any rotation, only translation and scaling
		auto objPos = Math::GetTranslationFromMatrix<DirectX::XMFLOAT3>(mat);
		auto scaling = Math::GetScalingFromMatrix<DirectX::XMFLOAT3>(mat);
		float angle = atan2f(objPos.x - camPos.x, objPos.z - camPos.z);
		return DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) *
			DirectX::XMMatrixRotationY(angle) *
			DirectX::XMMatrixTranslation(objPos.x, objPos.y, objPos.z);
	});
	if (renderInstances == 0)
		return;

	ID3D11Buffer * instances[] =
	{
		m_instanceBuffer.Get()
	};
	UINT stride = sizeof(DirectX::XMMATRIX);
	UINT offset = 0;
	m_d3d11Context->IASetVertexBuffers(1, 1, instances, &stride, &offset);

	renderer->OMBillboardBlend();
	BindMaterial(m_material, m_bindMaterialToShader);
	m_d3d11Context->DrawIndexedInstanced(GetIndexCount(),
		renderInstances, m_indexRange.begin, m_vertexRange.begin, 0);
	if (g_isDeveloper)
		g_drawCalls++;
	renderer->OMDefaultBlend();
}
