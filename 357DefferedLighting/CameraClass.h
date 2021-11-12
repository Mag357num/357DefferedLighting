#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <xnamath.h>

class CameraClass
{
private:
	float		mPostionX, mPostionY, mPostionZ;		//���������ռ��λ��
	float		mRotationX, mRotationY, mRotationZ;		//����ھֲ��ռ���ת��floatֵ( ���������תŷ���� )
	XMMATRIX	mViewMatrix;
	XMMATRIX	mBaseViewMatrix;						//����2D����( 2D Rendering )��BaseViewMatrix

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
		//������, λ������, �۲�����
		XMVECTOR up, eye, look;
		float yaw, pitch, roll; //ŷ����

		up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
		eye = XMVectorSet( mPostionX, mPostionY, mPostionZ, 0.0f );
		look = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );

		pitch = mRotationX * 0.0174532925f; //������, ���Ƕ�תΪ����, 0.0174532925fΪ1���Ӧ�Ļ���
		yaw = mRotationY * 0.0174532925f; //ƫ����
		roll = mRotationZ * 0.0174532925f; // ������

		//������ת���󣬲���ת�ֲ��ռ������Up������LookAt����
		XMMATRIX RotateMatrix = XMMatrixRotationRollPitchYaw( pitch, yaw, roll );
		look = XMVector3TransformCoord( look, RotateMatrix );
		up = XMVector3TransformCoord( up, RotateMatrix );

		//������Ӿֲ�ģ�Ϳռ��Ϊ����ռ�
		look = look + eye;

		//��󴴽�ViewMatrix
		mViewMatrix = XMMatrixLookAtLH( eye, look, up );

		//���������λ��
		eye = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
		look = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
		up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

		//������Ӿֲ�ģ�Ϳռ��Ϊ����ռ�
		look = look + eye;

		//��󴴽�BaseViewMatrix
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