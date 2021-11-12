#pragma once
#ifndef _MULTI_POINTLIGHT_SHADER_H
#define _MULTI_POINTLIGHT_SHADER_H

#include"Macro.h"
#include<d3d11.h>
#include<xnamath.h>
#include<D3DX11.h> //������Shader����ĺ���
#include<d3dcompiler.h>
#include<fstream>

using namespace std;

const int LIGHT_NUM = 100;

class MultiPointLightShader
{

private:
	//�任����������ṹ��
	struct CBMatrix
	{
		XMMATRIX mWorldMatrix;
		XMMATRIX mViewMatrix;
		XMMATRIX mProjMatrix;
		XMMATRIX mWorldInvTranposeMatirx;
	};

	//�ƹⳣ������ṹ��,ע���������XMFLOAT������XMVECTOR,16���ֽڵı���
	struct CBPointLightColor
	{
		XMFLOAT4 diffuseColor[LIGHT_NUM];
	};

	struct CBPointLightPos
	{
		XMFLOAT4 PointLightPos[LIGHT_NUM];
	};

private:
	ID3D11VertexShader*		md3dVertexShader;
	ID3D11PixelShader*		md3dPixelShader;
	ID3D11InputLayout*		md3dInputLayout;			//����VertexShader�����,���Ҫ����ColorShaderClass��,������D3DClass
	ID3D11Buffer*			mCBMatrixBuffer;			//�任����( ���� )����,��������Ҳ�����������
	ID3D11Buffer*			mCBLightBuffer;				//�ƹⳣ������
	ID3D11Buffer*			mCBPointLightPosBuffer;		//���Դλ�û���
	ID3D11SamplerState*		mSamplerState;				//����״̬( ������ʽ )

private:

	//��ʼ��Shader,���ڴ���InputLayout,VertexShader,PixelShader,��������
	bool InitializeShader( ID3D11Device*, HWND, WCHAR*, WCHAR* ); 

	//�ͷ�Shader
	bool ShutdownShader( );

	//���Shader�����ļ��Ĵ�����Ϣ
	void OutputShaderErrorMessage( ID3D10Blob*, HWND, WCHAR* );

	//����һ�������������ת��
	XMMATRIX GetInvenseTranspose( CXMMATRIX ma );

	bool SetShaderParameter( ID3D11DeviceContext*, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, FXMVECTOR[], FXMVECTOR[] );
	void RenderShader( ID3D11DeviceContext*, int );

public:
	MultiPointLightShader( );
	MultiPointLightShader( const MultiPointLightShader& );
	~MultiPointLightShader( );

public:
	bool Initialize( ID3D11Device*, HWND );
	void Shutdown( );
	bool Render( ID3D11DeviceContext*, int, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, FXMVECTOR diffuseColor[], FXMVECTOR PointLightPos[] );
};
#endif 
