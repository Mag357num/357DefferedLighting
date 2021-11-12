#pragma once
#ifndef _BITMAP_CLASS_H
#define _BITMAP_CLASS_H

#include<d3d11.h>
#include <xnamath.h>
#include"Macro.h" //���������ĺ�

/*Ĭ�������ͼƬ�Ĵ�С������Ĵ��ڲ�����ô�󣬲������Ͻǵ���������WIN32����ϵ( 0, 0 )��*/
class BitmapClass
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 color;
	};
private:
	ID3D11Buffer*	md3dVertexBuffer;	//���㻺��
	ID3D11Buffer*	md3dIndexBuffer;	//��������

	int mVertexCount;
	int mIndexCount;
	int mScrrenWidth, mScrrenHeight;

private:
	bool InitializeBuffer( ID3D11Device* d3dDevice );
	void ShutdownBuffer( );
	void SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext );
	bool UpdateBuffers( ID3D11DeviceContext* d3dDeviceContext );

public:
	BitmapClass( );
	~BitmapClass( );

public:
	//Initialize�Ǵ���Ԫ��, Render������Ԫ��, Shutdown��Release
	bool Initialize( ID3D11Device* d3dDevice, int ScrrenWidth, int ScrrenHeight );
	void Shutdown( );
	bool SetDataToIA( ID3D11DeviceContext* d3dDeviceContext );

	int GetIndexCount( ) { return mIndexCount; } //��������ֵ DrawIndexed( );
	
};
#endif 
