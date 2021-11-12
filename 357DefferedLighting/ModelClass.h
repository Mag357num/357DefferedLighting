#pragma once
#ifndef _MODEL_CLASS_H
#define _MODEL_CLASS_H

#include"TexClass.h"
#include<d3d11.h>
#include "Macro.h" //包含辅助的宏
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <xnamath.h>

using namespace std;

class ModelClass
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

private:
	ID3D11Buffer*	md3dVertexBuffer;	//顶点缓存
	ID3D11Buffer*	md3dIndexBuffer;	//索引缓存
	TexClass*		mTexClass;			//纹理类
	int				mVertexCount;
	int				mIndexCount;
	ModelType*		mModelData;			//在外部加载的模型数据

private:
	//加载各种缓存
	bool InitializeBuffer( ID3D11Device* d3dDevice );

	//释放各种缓存
	void ShutdownBuffer( );

	//设置( 渲染各种缓存 )
	void SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext );

	//加载纹理
	bool LoadTexture( ID3D11Device* d3dDevice, WCHAR* TexFileName ); 

	//释放纹理
	void ReleaseTexture( );

	//加载外部的模型数据
	bool LoadModelData( string ModelFileName );

	//释放外部的模型数据
	void ReleaseModelData( );

public:
	ModelClass( );
	~ModelClass( );

public:
	//Initialize是创建元素, Render是设置元素, Shutdown是Release
	bool Initialize( ID3D11Device* d3dDevice, WCHAR* TexFileName, string ModelFileName );
	void Shutdown( );
	void SetDataToIA( ID3D11DeviceContext* d3dDeviceContext );
	int GetIndexCount( ) { return mIndexCount; } //返回索引值 DrawIndexed( );
	ID3D11ShaderResourceView* GetTexture( ) { return mTexClass->GetTexture( );} //返回纹理资源
};
#endif