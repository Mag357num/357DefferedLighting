#pragma once
#ifndef _DEFFERRED_SHADER_CLASS_H
#define _DEFFERRED_SHADER_CLASS_H


/*����ӳ���ȾShader����RTT����������������ɫ�ͷ�����������Ŀ��������*/

#include"Macro.h"

#include<d3d11.h>
#include<D3DX11.h> //������Shader����ĺ���
#include<d3dcompiler.h>
#include<fstream>
#include <xnamath.h>

using namespace std;

class DefferredShaderClass
{

private:

	struct CBMatrix
	{
		XMMATRIX			mWorldMatrix;
		XMMATRIX			mViewMatrix;
		XMMATRIX			mProjMatrix;
		XMMATRIX			mWorldInvTranposeMatirx;
	};	//ShaderConst

private:
	ID3D11VertexShader*		md3dVertexShader;	//VS
	ID3D11PixelShader*		md3dPixelShader;	//PS
	ID3D11InputLayout*		md3dInputLayout;	//���벼��, ����VertexShader�����, ���Ҫ����ColorShaderClass��, ������D3DClass
	ID3D11Buffer*			mCBMatrixBuffer;	//SC, �任����������, ��������Ҳ�����������
	ID3D11SamplerState		*mSamplerState;		//����״̬( ������ʽ )

private:
	bool		InitializeShader( ID3D11Device*, HWND, WCHAR*, WCHAR* );		//����InputLayout, VS, PS, shaderConst
	void		OutputShaderErrorMessage( ID3D10Blob*, HWND, WCHAR* );			//���Shader�����ļ��Ĵ�����Ϣ
	XMMATRIX	GetInvenseTranspose( CXMMATRIX ma );							//����һ�������������ת��
	bool		SetShaderParameter( ID3D11DeviceContext*, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView* );
	void		RenderShader( ID3D11DeviceContext*, int );
	bool		ShutdownShader( );												//�ͷ�Shader

public:
	DefferredShaderClass( );
	~DefferredShaderClass( );

public:
	bool	Initialize( ID3D11Device*, HWND );
	void	Shutdown( );
	bool	Render( ID3D11DeviceContext*, int, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView* );
};
#endif