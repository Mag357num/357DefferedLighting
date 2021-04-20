#pragma once
#ifndef _DEFFERRED_BUFFER_CLASS_H
#define _DEFFERRED_BUFFER_CLASS_H

#include<Windows.h>
#include<D3D11.h>
#include"Macro.h"

const int BUFFER_COUNT = 3;
class GeometryBufferClass
{
private:
	ID3D11Texture2D*			mRenderTargetTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView*		mRenderTargetViewArray[BUFFER_COUNT];
	ID3D11ShaderResourceView*	mShaderResourceViewArray[BUFFER_COUNT];
	ID3D11Texture2D*			mDepthStencilBuffer;
	ID3D11DepthStencilView*		mDepthStencilView;
	D3D11_VIEWPORT				md3dViewport;

	int TextureWidth, TextureHeight;

public:
	GeometryBufferClass( );
	~GeometryBufferClass( );
	bool Initialize( ID3D11Device* d3dDevice, int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear );	
	void ShutDown( );

	void SetRenderTargetsAndViewports( ID3D11DeviceContext* deviceContext );
	void ClearRenderTargetView( ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha );
	ID3D11ShaderResourceView* GetShaderResourceView( int index );
};
#endif // !_RENDER_3D_MODEL_TO_TEXTURE_H
