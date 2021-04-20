#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <xnamath.h>

class CameraClass
{
private:
	float		mPostionX, mPostionY, mPostionZ;		//相机在世界空间的位置
	float		mRotationX, mRotationY, mRotationZ;		//相机在局部空间旋转的float值( 用于算出旋转欧拉角 )
	XMMATRIX	mViewMatrix;
	XMMATRIX	mBaseViewMatrix;						//绘制2D文字( 2D Rendering )的BaseViewMatrix

public:
	CameraClass( )
	{
		mPostionX = 0;
		mPostionY = 0;
		mPostionZ = -5.0f;
		mRotationX = 0;
		mRotationY = 0;
		mRotationZ = 0;
	}

	~CameraClass( )
	{

	}

public:

	void SetPostion( float x, float y, float z )
	{
		mPostionX = x;
		mPostionY = y;
		mPostionZ = z;
	}

	void SetRotation( float pitch, float yaw, float roll )
	{
		mRotationX = pitch;
		mRotationY = yaw;
		mRotationZ = roll;
	}

	XMVECTOR GetPostion( )
	{
		return XMVectorSet( mPostionX, mPostionY, mPostionZ, 0.0f );
	}

	XMVECTOR GetRotation( )
	{
		return XMVectorSet( mRotationX, mRotationY, mRotationZ, 0.0f );
	}

	void PrepareViewMatrix( )
	{
		//上向量, 位置向量, 观察向量
		XMVECTOR up, eye, look;
		float yaw, pitch, roll; //欧拉角

		up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		eye = XMVectorSet( mPostionX, mPostionY, mPostionZ, 0.0f );
		look = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );

		pitch = mRotationX * 0.0174532925f; //俯仰角, 将角度转为弧度, 0.0174532925f为1°对应的弧度
		yaw = mRotationY * 0.0174532925f; //偏航角
		roll = mRotationZ * 0.0174532925f; // 滚动角

		//创建旋转矩阵，并旋转局部空间相机的Up向量和LookAt向量
		XMMATRIX RotateMatrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
		look = XMVector3TransformCoord( look, RotateMatrix );
		up = XMVector3TransformCoord( up, RotateMatrix );

		//将相机从局部模型空间变为世界空间
		look = look + eye;

		//最后创建ViewMatrix
		mViewMatrix = XMMatrixLookAtLH( eye, look, up );

		//设置相机的位置
		eye = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
		look = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
		up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		//将相机从局部模型空间变为世界空间
		look = look + eye;

		//最后创建BaseViewMatrix
		mBaseViewMatrix = XMMatrixLookAtLH( eye, look, up );
	}

	XMMATRIX GetViewMatrix( )
	{
		return mViewMatrix;
	}

	XMMATRIX GetBaseViewMatrix( )
	{
		return mBaseViewMatrix;
	}

};
#endif