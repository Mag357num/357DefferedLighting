#pragma once
#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

#include <stdlib.h>
#include <time.h>
#include"D3DClass.h"
#include"CameraClass.h"
#include"LightShaderClass.h"
#include"MultiPointLightShader.h"
#include"GeometryBufferClass.h"
#include"DefferredShaderClass.h"
#include"ModelClass.h"
#include"LightClass.h"
#include"BitmapClass.h"

//ȫ�ֱ���
const bool		FULL_SCREEN = false;
const bool		VSYNC_ENABLE = true;	//�Ǿ����ܿ���Ⱦ��������֡��Ⱦ
const float		SCREEN_FAR = 1000.0f;	//�ӽ���Զ����
const float		SCREEN_NEAR = 0.1f;		//�ӽ��������

class GraphicsClass
{

private:
	
	D3DClass*				mD3D;				//D3D��
	CameraClass*			mCamera;			//����࣬���ڿ��Ƴ��������
	MultiPointLightShader*	mMultiLightShader;	//mMultiLightShader
	DefferredShaderClass*	mDefferredShader;	//DefferredShader
	GeometryBufferClass*	mGeometryBuffer;	//GeometryBuffer
	BitmapClass*			mBitmap;			//BitmapClass
	ModelClass*				mModel;				//���ڿ���VertexBuffer��IndexBuffer
	LightClass*				mPoints[LIGHT_NUM];

private:
	bool Render( float );
	bool RenderSceneToTexture( float );
	bool RenderTextureToBack( float );

public:
	GraphicsClass( );
	~GraphicsClass( );

public:
	bool Initialize( int ScreenWidth, int ScreenHeight, HWND hwnd );
	void Shutdown( );
	bool Frame( );
};
#endif // !_GRAPHICS_CLASS_H