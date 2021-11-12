#pragma once
#ifndef D3D_CLASS_H
#define D3D_CLASS_H

#include"Macro.h" //���������ĺ�
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
	bool						mVsyncEnable;					//�Ƿ���֡��Ⱦ
	int							mVideoCardMemory;				//�Կ��ڴ�
	char						mVideoCardDescription[128];		//�Կ�����

	ID3D11Device*				md3dDevice;						//�豸
	ID3D11DeviceContext*		md3dContext;					//�豸������
	IDXGISwapChain*				md3dSwapChain;					//������
	ID3D11RenderTargetView*		md3dRenderTargetView;			//��ȾĿ����ͼ
	ID3D11DepthStencilView*		md3dDepthStencilView;			//���ģ����ͼ
	ID3D11Texture2D*			md3dDepthStencilBuffer;			//���ģ�建��
	ID3D11DepthStencilState*	md3dDepthStencilState;			//���ģ��״̬
	ID3D11DepthStencilState*	md3dDisableDepthStencilState;	//�ر����ģ�建���״̬
	ID3D11RasterizerState*		md3dRasterizerState;			//��դ��״̬
	D3D11_VIEWPORT				md3dViewport;					//�ӿ�

	XMMATRIX					mWorldMatrix;					//����任����
	XMMATRIX					mOrthoMatrix;					//��������
	XMMATRIX					mProjMatrix;					//ͶӰ����


public:
	//����, ��������, ��������
	D3DClass( );
	~D3DClass( );

	//D3DClass��ʼ������
	bool Initialize( int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear );

	//�ر�D3DClass����
	void Shutdown( );

	//���Ƴ�������
	void BeginScene( float red, float green, float blue, float alpha );
	void EndScene( );

	//Get����
	ID3D11Device* GetDevice( ) { return md3dDevice; }
	ID3D11DeviceContext* GetDeviceContext( ){ return md3dContext; }
	XMMATRIX GetWorldMatrix( ) { return mWorldMatrix; }
	XMMATRIX GetOrthoMatrix( ) { return mOrthoMatrix; }
	XMMATRIX GetProjMatrix( ) { return mProjMatrix; };

	void GetVideoCardInfo( char*, int& ); //��ȡ�Կ���Ϣ

 //Set����
	void SetBackBufferRender( ); //���ñ��󻺴���Ϊ��ȾĿ�꣬������ڡ���Ⱦ���������֮���ڵ��ã����������Ϊ�������ǽ�ͼ����Ⱦ�����󻺴����������
	void SetViewPort( ); //���ø��ӿ�

	//�򿪺͹ر�ZTest
	void TurnOnZBuffer( );
	void TurnOffZBuffer( );
};
#endif // !D3D_CLASS_H
