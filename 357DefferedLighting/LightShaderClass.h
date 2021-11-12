#pragma once
#ifndef _LIGHT_SHADER_CLASS_H
#define _LIGHT_SHADER_CLASS_H

#include"Macro.h"

#include<d3d11.h>
#include<D3DX11.h> //������Shader����ĺ���
#include<d3dcompiler.h>
#include<fstream>
#include <xnamath.h>

using namespace std;

class LightShaderClass
{

private:
	//�任����������ṹ��
	struct CBMatrix
	{
		XMMATRIX mWorldMatrix;
		XMMATRIX mViewMatrix;
		XMMATRIX mProjMatrix;
	};

	//�ƹⳣ������ṹ��, ע���������XMFLOAT������XMVECTOR, 16���ֽڵı���
	struct CBLight //����shader��ĳ��������Ӧλ�õ�ֵ���Ӧ
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 LightDireciton;
		float pad; //����ϵ��
	};

private:
	ID3D11VertexShader*		md3dVertexShader;
	ID3D11PixelShader*		md3dPixelShader;
	ID3D11InputLayout*		md3dInputLayout;		//����VertexShader�����, ���Ҫ����ColorShaderClass��, ������D3DClass
	ID3D11Buffer*			mCBMatrixBuffer;		//�任����( ���� )����, ��������Ҳ�����������
	ID3D11Buffer*			mCBLightBuffer;			//�ƹⳣ������
	ID3D11Buffer*			mCBPointLightPosBuffer;	//�ƹⳣ������
	ID3D11SamplerState		*mSamplerState;			//����״̬( ������ʽ )

private:

	//��ʼ��Shader, ���ڴ���InputLayout, VertexShader, PixelShader, ��������
	bool InitializeShader( ID3D11Device*, HWND, WCHAR*, WCHAR* ); 

	//�ͷ�Shader
	bool ShutdownShader( );

	//���Shader�����ļ��Ĵ�����Ϣ
	void OutputShaderErrorMessage( ID3D10Blob*, HWND, WCHAR* );

	//����һ�������������ת��
	XMMATRIX GetInvenseTranspose( CXMMATRIX ma );

	bool SetShaderParameter( ID3D11DeviceContext*, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, FXMVECTOR, FXMVECTOR );
	void RenderShader( ID3D11DeviceContext*, int );

public:
	LightShaderClass( );
	~LightShaderClass( );

public:
	bool Initialize( ID3D11Device*, HWND );
	void Shutdown( );
	bool Render( ID3D11DeviceContext*, int, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, FXMVECTOR diffuseColor, FXMVECTOR LightDirection );
};
#endif