#pragma once
#ifndef _BITMAP_CLASS_H
#define _BITMAP_CLASS_H

#include<d3d11.h>
#include <xnamath.h>
#include"Macro.h" //包含辅助的宏

/*默认情况下图片的大小跟传入的窗口参数这么大，并且左上角的坐标是在WIN32坐标系( 0, 0 )处*/
class BitmapClass
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 color;
	};
private:
	ID3D11Buffer*	md3dVertexBuffer;	//顶点缓存
	ID3D11Buffer*	md3dIndexBuffer;	//索引缓存

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
	//Initialize是创建元素, Render是设置元素, Shutdown是Release
	bool Initialize( ID3D11Device* d3dDevice, int ScrrenWidth, int ScrrenHeight );
	void Shutdown( );
	bool SetDataToIA( ID3D11DeviceContext* d3dDeviceContext );

	int GetIndexCount( ) { return mIndexCount; } //返回索引值 DrawIndexed( );
	
};
#endif 
