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

//全局变量
const bool		FULL_SCREEN = false;
const bool		VSYNC_ENABLE = true;	//是尽可能快渲染还是限制帧渲染
const float		SCREEN_FAR = 1000.0f;	//视截体远裁面
const float		SCREEN_NEAR = 0.1f;		//视截体近裁面

class GraphicsClass
{

private:
	
	D3DClass*				mD3D;				//D3D类
	CameraClass*			mCamera;			//相机类，用于控制场景的相机
	MultiPointLightShader*	mMultiLightShader;	//mMultiLightShader
	DefferredShaderClass*	mDefferredShader;	//DefferredShader
	GeometryBufferClass*	mGeometryBuffer;	//GeometryBuffer
	BitmapClass*			mBitmap;			//BitmapClass
	ModelClass*				mModel;				//用于控制VertexBuffer和IndexBuffer
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