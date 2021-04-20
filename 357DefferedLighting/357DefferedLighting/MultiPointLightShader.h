#pragma once
#ifndef _MULTI_POINTLIGHT_SHADER_H
#define _MULTI_POINTLIGHT_SHADER_H

#include"Macro.h"
#include<d3d11.h>
#include<xnamath.h>
#include<D3DX11.h> //含编译Shader程序的函数
#include<d3dcompiler.h>
#include<fstream>

using namespace std;

const int LIGHT_NUM = 100;

class MultiPointLightShader
{

private:
	//变换矩阵常量缓存结构体
	struct CBMatrix
	{
		XMMATRIX mWorldMatrix;
		XMMATRIX mViewMatrix;
		XMMATRIX mProjMatrix;
		XMMATRIX mWorldInvTranposeMatirx;
	};

	//灯光常量缓存结构体,注意类变量用XMFLOAT而不是XMVECTOR,16个字节的倍数
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
	ID3D11InputLayout*		md3dInputLayout;			//这与VertexShader相关联,因此要放在ColorShaderClass里,而不是D3DClass
	ID3D11Buffer*			mCBMatrixBuffer;			//变换矩阵( 常量 )缓存,顶点索引也是用这个类型
	ID3D11Buffer*			mCBLightBuffer;				//灯光常量缓存
	ID3D11Buffer*			mCBPointLightPosBuffer;		//点光源位置缓存
	ID3D11SamplerState*		mSamplerState;				//采样状态( 采样方式 )

private:

	//初始化Shader,用于创建InputLayout,VertexShader,PixelShader,常量缓存
	bool InitializeShader( ID3D11Device*, HWND, WCHAR*, WCHAR* ); 

	//释放Shader
	bool ShutdownShader( );

	//输出Shader编译文件的错误信息
	void OutputShaderErrorMessage( ID3D10Blob*, HWND, WCHAR* );

	//返回一个矩阵的逆矩阵的转置
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
