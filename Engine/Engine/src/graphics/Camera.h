#pragma once

#include "../input/Mouse.h"
#include "../input/Keyboard.h"
#include "interfaces/ICamera.h"


class Camera : public ICamera, public IAlignedObject
{
	static const DirectX::XMVECTOR Forward;
	static const DirectX::XMVECTOR Right;
	static const DirectX::XMVECTOR Up;
	static const DirectX::XMVECTOR DefaultPosition;
	static const DirectX::XMMATRIX DefaultView;
	static constexpr const float Friction = 0.2f;
	static constexpr const float CamSpeed = 5.f;
	static constexpr const float MaxCamSpeed = 50.f;
	static constexpr const float RotationFriction = 0.2f;
public:
	Camera() = default;
	Camera(float FOV, float HByW, float NearZ, float FarZ);
	~Camera();

public:
	void Update(float frameTime, float horizontalMouseMove, float verticalMouseMove);
	void Construct();

	void WalkForward(float frameTime);
	void WalkBackward(float frameTime);
	void StrafeRight(float frameTime);
	void StrafeLeft(float frameTime);

public:
	inline DirectX::XMMATRIX& GetView() override { return mView; };
	inline DirectX::XMMATRIX& GetProjection() override { return mProjection; };
	inline float GetNearZ() const override { return mNearZ; };
	inline float GetFarZ() const override { return mFarZ; };
	inline DirectX::XMFLOAT3 GetPosition() const override
	{
		DirectX::XMFLOAT3 Pos; DirectX::XMStoreFloat3(&Pos, mPosition); return Pos;
	};
	inline DirectX::XMFLOAT3 GetDirection()
	{
		DirectX::XMFLOAT3 Dir; DirectX::XMStoreFloat3(&Dir, mDirection); return Dir;
	};
	inline DirectX::XMFLOAT3 GetCamRotation()
	{
		return DirectX::XMFLOAT3(mPitch, mYaw, mRoll);
	};
	inline void SetPosition(DirectX::FXMVECTOR& position)
	{
		mPosition = position;
	};
	inline void SetDirection(DirectX::FXMVECTOR& direction)
	{
		mDirection = direction;
	};
	inline void SetCamRotation(const DirectX::XMFLOAT3& rotation)
	{
		mPitch = rotation.x;
		mYaw = rotation.y;
		mRoll = rotation.z;
	}

private:
	DirectX::XMVECTOR mDirection = Forward;
	DirectX::XMVECTOR mRight = Right;
	DirectX::XMVECTOR mUp = Up;
	DirectX::XMVECTOR mPosition = DefaultPosition;

	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;

	float mForwardAcceleration = 0.0f;
	float mRightAcceleration = 0.0f;

	float mYRotationAcceleration = 0.0f;
	float mZRotationAcceleration = 0.0f;

	float mNearZ = 0.0f;
	float mFarZ = 0.0f;

	float mYaw = 0;
	float mPitch = 0;
	float mRoll = 0;
};
