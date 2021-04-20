#pragma once
#ifndef D3D_CLASS_H
#define D3D_CLASS_H

#include"Macro.h" //包含辅助的宏
#include<Windows.h>
#include<D3Dcommon.h>
#include<D3DX11.h>
#include<iostream>
#include<istream>
#include<ostream>
#include<strstream>
#include<fstream>
#include <xnamath.h>

using namespace std;

class D3DClass
{
private:
	bool						mVsyncEnable;					//是否限帧渲染
	int							mVideoCardMemory;				//显卡内存
	char						mVideoCardDescription[128];		//显卡名字

	ID3D11Device*				md3dDevice;						//设备
	ID3D11DeviceContext*		md3dContext;					//设备上下文
	IDXGISwapChain*				md3dSwapChain;					//交换链
	ID3D11RenderTargetView*		md3dRenderTargetView;			//渲染目标视图
	ID3D11DepthStencilView*		md3dDepthStencilView;			//深度模板视图
	ID3D11Texture2D*			md3dDepthStencilBuffer;			//深度模板缓存
	ID3D11DepthStencilState*	md3dDepthStencilState;			//深度模板状态
	ID3D11DepthStencilState*	md3dDisableDepthStencilState;	//关闭深度模板缓存的状态
	ID3D11RasterizerState*		md3dRasterizerState;			//光栅化状态
	D3D11_VIEWPORT				md3dViewport;					//视口

	XMMATRIX					mWorldMatrix;					//世界变换矩阵
	XMMATRIX					mOrthoMatrix;					//正交矩阵
	XMMATRIX					mProjMatrix;					//投影矩阵


public:
	//构造, 拷贝构造, 析构函数
	D3DClass( );
	~D3DClass( );

	//D3DClass初始化函数
	bool Initialize( int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear );

	//关闭D3DClass函数
	void Shutdown( );

	//绘制场景函数
	void BeginScene( float red, float green, float blue, float alpha );
	void EndScene( );

	//Get函数
	ID3D11Device* GetDevice( ) { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext( ){ return md3dContext; }
	XMMATRIX GetWorldMatrix( ) { return mWorldMatrix; }
	XMMATRIX GetOrthoMatrix( ) { return mOrthoMatrix; }
	XMMATRIX GetProjMatrix( ) { return mProjMatrix; };

	void GetVideoCardInfo( char*, int& ); //获取显卡信息

 //Set函数
	void SetBackBufferRender( ); //设置背后缓存作为渲染目标，这个将在“渲染到纹理”完成之后在调用，这个函数是为了让我们将图案渲染到背后缓存而不是纹理
	void SetViewPort( ); //重置该视口

	//打开和关闭ZTest
	void TurnOnZBuffer( );
	void TurnOffZBuffer( );
};
#endif // !D3D_CLASS_H
