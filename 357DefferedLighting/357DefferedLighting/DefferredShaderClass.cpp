#include"DefferredShaderClass.h"

DefferredShaderClass::DefferredShaderClass( )
{
	md3dVertexShader = NULL; //VS
	md3dPixelShader = NULL; //PS
	md3dInputLayout = NULL;
	mCBMatrixBuffer = NULL;
}

DefferredShaderClass::~DefferredShaderClass( )
{

}

bool DefferredShaderClass::Initialize( ID3D11Device* d3dDevice, HWND hwnd )
{
	bool result;
	result = InitializeShader( d3dDevice, hwnd, ( WCHAR* )L"MyShader/DefferredShader.fx" , ( WCHAR* )L"MyShader/DefferredShader.fx" );
	if ( !result )
		return false;

	return true;
}

void DefferredShaderClass::Shutdown( )
{
	ShutdownShader( );
}

bool DefferredShaderClass::Render( ID3D11DeviceContext* d3dDeviceContext, int indexCount, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix, ID3D11ShaderResourceView * texture )
{
	bool result;

	//设置ShaderConst和纹理资源
	result = SetShaderParameter( d3dDeviceContext, worldMatrix, viewMatrix, ProjMatrix, texture );
	if ( !result )
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	RenderShader( d3dDeviceContext, indexCount );

	return true;
}

bool DefferredShaderClass::InitializeShader( ID3D11Device* d3dDevice, HWND hwnd, WCHAR* VSFileName, WCHAR* PSFileName )
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;

	//第一, 初始化参数
	errorMessage = NULL;
	VertexShaderBuffer = NULL;
	PixelShaderBuffer = NULL;

	//第二, 编译VertexShader代码, 并创建VertexShader. "VS": 函数名称, "vs_5_0": shader模型, D3DCOMPILE_ENABLE_STRICTNESS: 严格语法形式地编译
	result = D3DX11CompileFromFile( VSFileName, NULL, NULL, "VS", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &VertexShaderBuffer, &errorMessage, NULL );
	if ( FAILED( result ) )
	{
		//存在错误信息
		if ( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, VSFileName );
		}
		//不存在错误信息, 也就是没有找到Shader文件
		else
		{
			MessageBox( hwnd, L"can not find VS file", L"error", MB_OK );
		}
	}

	HR( d3dDevice->CreateVertexShader( VertexShaderBuffer->GetBufferPointer( ), VertexShaderBuffer->GetBufferSize( ), NULL, &md3dVertexShader ) );


	//第三, 编译PixelShader, 并创建PixelShader
	result = D3DX11CompileFromFile( PSFileName, NULL, NULL, "PS", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &PixelShaderBuffer, &errorMessage, NULL );
	if ( FAILED( result ) )
	{
		//存在错误信息
		if ( errorMessage )
		{
			OutputShaderErrorMessage( errorMessage, hwnd, PSFileName );
		}
		//不存在错误信息, 也就是没有找到Shader文件
		else
		{
			MessageBox( hwnd, L"can not find PS file", L"error", MB_OK );
		}
	}

	HR( d3dDevice->CreatePixelShader( PixelShaderBuffer->GetBufferPointer( ), PixelShaderBuffer->GetBufferSize( ), NULL, &md3dPixelShader ) );

	//第四, 填充输入布局描述, 创建输入布局
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, //96位即12个字节
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	}; // 语义名, 语义索引, 数据格式, 输入槽索引( 0-15 ), 初始位置( 字节偏移量 ), 输入类型, 示例数据绘制次数

	unsigned int numElements = sizeof( VertexInputLayout ) / sizeof( VertexInputLayout[0] ); //布局数量

	HR( d3dDevice->CreateInputLayout( VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer( ), VertexShaderBuffer->GetBufferSize( ), &md3dInputLayout ) );

	//第五, 释放VertexShaderBuffer和PixelShaderBuffer
	VertexShaderBuffer->Release( );
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release( );
	PixelShaderBuffer = NULL;

	//第六, 设置变换矩阵常量缓存描述, 并创建矩阵常量缓存
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory( &matrixBufferDesc, sizeof( matrixBufferDesc ) );
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof( CBMatrix ); //结构体大小, 必须为16字节倍数
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;

	HR( d3dDevice->CreateBuffer( &matrixBufferDesc, NULL, &mCBMatrixBuffer ) );

	//第七, 填充采样描述, 并且创建采样状态
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //都是线性插值( 三种方式, 点过滤, 线性过滤, 异性过滤 )
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR( d3dDevice->CreateSamplerState( &samplerDesc, &mSamplerState ) );

	return true;
}

bool DefferredShaderClass::ShutdownShader( )
{
	HR( mCBMatrixBuffer );
	HR( md3dInputLayout );
	HR( md3dPixelShader );
	HR( md3dVertexShader );
	return true;
}

void DefferredShaderClass::OutputShaderErrorMessage( ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename )
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// 获取指向错误信息文本的指针
	compileErrors = ( char* )( errorMessage->GetBufferPointer( ) );

	// 获取错误信息文本的长度
	bufferSize = errorMessage->GetBufferSize( );

	// 创建一个txt, 用于写入错误信息
	fout.open( "shader-error.txt" );

	//想txt文件写入错误信息
	for ( i = 0; i<bufferSize; i++ )
	{
		fout << compileErrors[i];
	}

	// 关闭文件
	fout.close( );

	// Release the error message.
	errorMessage->Release( );
	errorMessage = 0;

	//弹出提醒的小窗口
	MessageBox( hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK );
}

bool DefferredShaderClass::SetShaderParameter( ID3D11DeviceContext* d3dDeviceContext, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix, ID3D11ShaderResourceView* texture )
{

	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置, 在传入常量缓存前进行转置, 因为GPU对矩阵数据会自动进行一次转置
	CBMatrix cb;
	XMMATRIX worldMa = XMMatrixTranspose( worldMatrix );
	XMMATRIX viewMa = XMMatrixTranspose( viewMatrix );
	XMMATRIX ProjMa = XMMatrixTranspose( ProjMatrix );
	XMMATRIX worldInvTranspose = XMMatrixTranspose( GetInvenseTranspose( worldMatrix ) ); //世界矩阵的逆矩阵的转置
	cb.mWorldMatrix = worldMa;
	cb.mViewMatrix = viewMa;
	cb.mProjMatrix = ProjMa;
	cb.mWorldInvTranposeMatirx = worldInvTranspose;
	d3dDeviceContext->UpdateSubresource( mCBMatrixBuffer, 0, NULL, &cb, 0, 0 ); //数据拷贝

	//第二, 设置在VertexShader的常量缓存的值( 带着更新的值 )
	d3dDeviceContext->VSSetConstantBuffers( 0, 1, &mCBMatrixBuffer );

	//第三, 设置在PixelShader的纹理资源
	d3dDeviceContext->PSSetShaderResources( 0, 1, &texture );

	return true;
}

void DefferredShaderClass::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount )
{
	//设置顶点输入布局
	deviceContext->IASetInputLayout( md3dInputLayout );

	//设置VertexShader和PixelShader
	deviceContext->VSSetShader( md3dVertexShader, NULL, 0 );
	deviceContext->PSSetShader( md3dPixelShader, NULL, 0 );

	//设置采样状态
	deviceContext->PSSetSamplers( 0, 1, &mSamplerState ); //S0注册 对应0

	//渲染三角形
	deviceContext->DrawIndexed( indexCount, 0, 0 );
}

//返回一个矩阵的逆矩阵的转置
XMMATRIX DefferredShaderClass::GetInvenseTranspose( CXMMATRIX ma )
{
	XMMATRIX A = ma;

	//将矩阵A的第四行置零, 因为向量是无法进行平移的
	A.r[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

	XMVECTOR det = XMMatrixDeterminant( A );
	XMMATRIX MaInvense = XMMatrixInverse( &det, A );
	return XMMatrixTranspose( MaInvense );
}