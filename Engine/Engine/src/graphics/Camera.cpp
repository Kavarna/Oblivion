#include "Camera.h"
#include "../scripting/LuaManager.h"


const DirectX::XMVECTOR Camera::Forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const DirectX::XMVECTOR Camera::Right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Camera::Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const DirectX::XMVECTOR Camera::DefaultPosition = DirectX::XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f);
const DirectX::XMMATRIX Camera::DefaultView = DirectX::XMMatrixLookToLH(DefaultPosition, Forward, Up);

Camera::Camera(float FOV, float HByW, float NearZ, float FarZ) :
	mNearZ(NearZ),
	mFarZ(FarZ)
{
	mProjection = DirectX::XMMatrixPerspectiveFovLH(FOV, HByW, NearZ, FarZ);
}

Camera::~Camera()
{}

void Camera::LuaRegister()
{
	US_NS_LUA;

	getGlobalNamespace()
		.beginNamespace("Oblivion")
			.deriveClass<Camera, ICamera>("Camera")
				//.addConstructor<void(*)()>()
				//.addConstructor<void(*)(float, float, float, float)>()
				.addFunction("RenderDebug", &Camera::RenderDebug)
				.addFunction("GetNearZ", &Camera::GetNearZ)
				.addFunction("GetFarZ", &Camera::GetFarZ)
				.addFunction("GetDirection", &Camera::GetDirection)
				.addFunction("GetPosition", &Camera::GetPosition)
				.addFunction("GetView", &Camera::GetView)
				.addFunction("GetProjection", &Camera::GetProjection)
			.endClass()
		.endNamespace();
}

void Camera::Update(float frameTime, float horizontalMouseMove, float verticalMouseMove)
{
	mYRotationAcceleration += horizontalMouseMove * 0.001f;
	mZRotationAcceleration += verticalMouseMove * 0.001f;

	mYaw += ( mYRotationAcceleration * ( 1.0f - RotationFriction ) );
	mYRotationAcceleration *= ( 1.0f - RotationFriction );

	mPitch += ( mZRotationAcceleration * ( 1.0f - RotationFriction ) );
	mZRotationAcceleration *= ( 1.0f - RotationFriction );

	DX::clamp(mForwardAcceleration, -MaxCamSpeed, MaxCamSpeed);
	DX::clamp(mRightAcceleration, -MaxCamSpeed, MaxCamSpeed);

	Construct();

	mForwardAcceleration *= ( 1.0f - Friction );
	mRightAcceleration *= ( 1.0f - Friction );
}

void Camera::Construct()
{
	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0.0f);
	mDirection = DirectX::XMVector3TransformCoord(Forward, Rotation);
	mRight = DirectX::XMVector3TransformCoord(Right, Rotation);
	mUp = DirectX::XMVector3Cross(mDirection, mRight);


	mPosition = DirectX::XMVectorAdd(mPosition, DirectX::XMVectorScale(mDirection, mForwardAcceleration));
	mPosition = DirectX::XMVectorAdd(mPosition, DirectX::XMVectorScale(mRight, mRightAcceleration));

	mView = DirectX::XMMatrixLookToLH(mPosition, mDirection, mUp);

	BuildViewFrustum();
}

void Camera::WalkForward(float frameTime)
{
	mForwardAcceleration += CamSpeed * frameTime;
}

void Camera::WalkBackward(float frameTime)
{
	mForwardAcceleration -= CamSpeed * frameTime;
}

void Camera::StrafeRight(float frameTime)
{
	mRightAcceleration += CamSpeed * frameTime;
}

void Camera::StrafeLeft(float frameTime)
{
	mRightAcceleration -= CamSpeed * frameTime;
}
