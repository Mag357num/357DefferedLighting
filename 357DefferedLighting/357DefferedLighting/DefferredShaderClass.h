#pragma once
#ifndef _DEFFERRED_SHADER_CLASS_H
#define _DEFFERRED_SHADER_CLASS_H


/*这个延迟渲染Shader将和RTT技术配合用于输出颜色和法向量到两张目的纹理中*/

#include"Macro.h"

#include<d3d11.h>
#include<D3DX11.h> //含编译Shader程序的函数
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
	ID3D11InputLayout*		md3dInputLayout;	//输入布局, 这与VertexShader相关联, 因此要放在ColorShaderClass里, 而不是D3DClass
	ID3D11Buffer*			mCBMatrixBuffer;	//SC, 变换矩阵常量缓存, 顶点索引也是用这个类型
	ID3D11SamplerState		*mSamplerState;		//采样状态( 采样方式 )

private:
	bool		InitializeShader( ID3D11Device*, HWND, WCHAR*, WCHAR* );		//创建InputLayout, VS, PS, shaderConst
	void		OutputShaderErrorMessage( ID3D10Blob*, HWND, WCHAR* );			//输出Shader编译文件的错误信息
	XMMATRIX	GetInvenseTranspose( CXMMATRIX ma );							//返回一个矩阵的逆矩阵的转置
	bool		SetShaderParameter( ID3D11DeviceContext*, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView* );
	void		RenderShader( ID3D11DeviceContext*, int );
	bool		ShutdownShader( );												//释放Shader

public:
	DefferredShaderClass( );
	~DefferredShaderClass( );

public:
	bool	Initialize( ID3D11Device*, HWND );
	void	Shutdown( );
	bool	Render( ID3D11DeviceContext*, int, CXMMATRIX, CXMMATRIX, CXMMATRIX, ID3D11ShaderResourceView* );
};
#endif