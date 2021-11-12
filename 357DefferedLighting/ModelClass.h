#pragma once
#ifndef _MODEL_CLASS_H
#define _MODEL_CLASS_H

#include"TexClass.h"
#include<d3d11.h>
#include "Macro.h" //���������ĺ�
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
	ID3D11Buffer*	md3dVertexBuffer;	//���㻺��
	ID3D11Buffer*	md3dIndexBuffer;	//��������
	TexClass*		mTexClass;			//������
	int				mVertexCount;
	int				mIndexCount;
	ModelType*		mModelData;			//���ⲿ���ص�ģ������

private:
	//���ظ��ֻ���
	bool InitializeBuffer( ID3D11Device* d3dDevice );

	//�ͷŸ��ֻ���
	void ShutdownBuffer( );

	//����( ��Ⱦ���ֻ��� )
	void SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext );

	//��������
	bool LoadTexture( ID3D11Device* d3dDevice, WCHAR* TexFileName ); 

	//�ͷ�����
	void ReleaseTexture( );

	//�����ⲿ��ģ������
	bool LoadModelData( string ModelFileName );

	//�ͷ��ⲿ��ģ������
	void ReleaseModelData( );

public:
	ModelClass( );
	~ModelClass( );

public:
	//Initialize�Ǵ���Ԫ��, Render������Ԫ��, Shutdown��Release
	bool Initialize( ID3D11Device* d3dDevice, WCHAR* TexFileName, string ModelFileName );
	void Shutdown( );
	void SetDataToIA( ID3D11DeviceContext* d3dDeviceContext );
	int GetIndexCount( ) { return mIndexCount; } //��������ֵ DrawIndexed( );
	ID3D11ShaderResourceView* GetTexture( ) { return mTexClass->GetTexture( );} //����������Դ
};
#endif