#include"MultiPointLightShader.h"

MultiPointLightShader::MultiPointLightShader( )
{
	md3dVertexShader = NULL;
	md3dPixelShader = NULL;
	md3dInputLayout = NULL;
	mCBMatrixBuffer = NULL;
	mCBLightBuffer = NULL;
	mCBPointLightPosBuffer = NULL;
}


MultiPointLightShader::MultiPointLightShader( const MultiPointLightShader& )
{

}

MultiPointLightShader::~MultiPointLightShader( )
{

}


bool MultiPointLightShader::Initialize( ID3D11Device* d3dDevice, HWND hwnd )
{
	bool result;
	result = InitializeShader( d3dDevice, hwnd, ( WCHAR* )L"MyShader/MultiPointLight.fx", ( WCHAR* )L"MyShader/MultiPointLight.fx" );
	if ( !result )
		return false;

	return true;
}

void MultiPointLightShader::Shutdown( )
{
	ShutdownShader( );
}

bool MultiPointLightShader::Render( ID3D11DeviceContext* d3dDeviceContext, int indexCount, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix,
	ID3D11ShaderResourceView* ColorTexture, ID3D11ShaderResourceView* NormalTexture, ID3D11ShaderResourceView* WposTexture, XMVECTOR diffuseColor[], XMVECTOR PointLightPos[] )
{
	bool result;

	//设置ShaderConst和纹理资源
	result = SetShaderParameter( d3dDeviceContext, worldMatrix, viewMatrix, ProjMatrix, ColorTexture, NormalTexture, WposTexture, diffuseColor, PointLightPos );
	if ( !result )
		return false;

	//设置VertexShader PixelShader InputLayout SamplerState
	RenderShader( d3dDeviceContext, indexCount );

	return true;
}

bool MultiPointLightShader::InitializeShader( ID3D11Device* d3dDevice, HWND hwnd, WCHAR* VSFileName, WCHAR* PSFileName )
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;
	ID3D10Blob* PixelShaderBuffer;

	//第一, 初始化参数
	errorMessage = NULL;
	VertexShaderBuffer = NULL;
	PixelShaderBuffer = NULL;

	//第二, 编译VertexShader代码, 并创建VertexShader
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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, // 96位即12个字节
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof( VertexInputLayout ) / sizeof( VertexInputLayout[0] ); //布局数量

	HR( d3dDevice->CreateInputLayout( VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer( ), VertexShaderBuffer->GetBufferSize( ), &md3dInputLayout ) );

	//第五, 释放VertexShaderBuffer和PixelShaderBuffer
	VertexShaderBuffer->Release( );
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release( );
	PixelShaderBuffer = NULL;

	//第六, 设置( 变换矩阵常量 )缓存描述, 并创建矩阵常量缓存
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory( &matrixBufferDesc, sizeof( matrixBufferDesc ) );
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof( CBMatrix ); //结构体大小, 必须为16字节倍数
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0; 

	HR( d3dDevice->CreateBuffer( &matrixBufferDesc, NULL, &mCBMatrixBuffer ) );

	//第七, 设置（灯光常量缓存）缓存描述，并创建灯光常量缓存, 复制粘贴的时候别把LightBufferDesc和matrixBufferDesc搞错了
	D3D11_BUFFER_DESC LightBufferDesc;
	ZeroMemory( &LightBufferDesc, sizeof( LightBufferDesc ) );
	LightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	LightBufferDesc.ByteWidth = sizeof( CBPointLightColor ); //结构体大小, 必须为16字节倍数
	LightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	LightBufferDesc.CPUAccessFlags = 0;

	HR( d3dDevice->CreateBuffer( &LightBufferDesc, NULL, &mCBLightBuffer ) );


	//第八, 设置（灯光常量缓存）缓存描述，并创建灯光常量缓存, 复制粘贴的时候别把LightBufferDesc和matrixBufferDesc搞错了
	D3D11_BUFFER_DESC PoinjtLightPosBufferDesc;
	ZeroMemory( &PoinjtLightPosBufferDesc, sizeof( PoinjtLightPosBufferDesc ) );
	PoinjtLightPosBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	PoinjtLightPosBufferDesc.ByteWidth = sizeof( CBPointLightPos ); //结构体大小,必须为16字节倍数
	PoinjtLightPosBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	PoinjtLightPosBufferDesc.CPUAccessFlags = 0;

	HR( d3dDevice->CreateBuffer( &PoinjtLightPosBufferDesc, NULL, &mCBPointLightPosBuffer ) );

	//第九, 填充采样描述, 并且创建采样状态
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

bool MultiPointLightShader::ShutdownShader( )
{
	ReleaseCOM( mCBPointLightPosBuffer );
	ReleaseCOM( mCBMatrixBuffer );
	ReleaseCOM( mCBLightBuffer );
	ReleaseCOM( md3dInputLayout );
	ReleaseCOM( md3dPixelShader );
	ReleaseCOM( md3dVertexShader );
	return true;
}

void MultiPointLightShader::OutputShaderErrorMessage( ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename )
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// 获取指向错误信息文本的指针
	compileErrors = ( char* )( errorMessage->GetBufferPointer( ) );

	// 获取错误信息文本的长度
	bufferSize = errorMessage->GetBufferSize( );

	// 创建一个txt,用于写入错误信息
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

bool MultiPointLightShader::SetShaderParameter( ID3D11DeviceContext* d3dDeviceContext, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix,
	ID3D11ShaderResourceView* ColorTexture, ID3D11ShaderResourceView* NormalTexture, ID3D11ShaderResourceView* WposTexture, XMVECTOR diffuseColor[], XMVECTOR PointLightPos[] )
{
	//第一，更新变换矩阵常量缓存的值
	//将矩阵转置,在传入常量缓存前进行转置,因为GPU对矩阵数据会自动进行一次转置
	CBMatrix cb;
	XMMATRIX worldMa = XMMatrixTranspose( worldMatrix );
	XMMATRIX viewMa = XMMatrixTranspose( viewMatrix );
	XMMATRIX ProjMa = XMMatrixTranspose( ProjMatrix );
	cb.mWorldMatrix = worldMa;
	cb.mViewMatrix = viewMa;
	cb.mProjMatrix = ProjMa;
	d3dDeviceContext->UpdateSubresource( mCBMatrixBuffer, 0, NULL, &cb, 0, 0 ); //数据拷贝

	//解锁顶点缓存
	d3dDeviceContext->Unmap( mCBMatrixBuffer, 0 );

	//第二，更新灯光常量缓存的值
	//设置在顶点缓存中常量缓存的位置,注册号
	CBPointLightColor cbLight;
	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		XMStoreFloat4( &cbLight.diffuseColor[i], diffuseColor[i] );
	}
	d3dDeviceContext->UpdateSubresource( mCBLightBuffer, 0, NULL, &cbLight, 0, 0 ); //数据拷贝

	//第三,更新点光源位置缓存的值
	CBPointLightPos cbPointLightPos;
	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		XMStoreFloat4( &cbPointLightPos.PointLightPos[i],PointLightPos[i] );
	}
	d3dDeviceContext->UpdateSubresource( mCBPointLightPosBuffer, 0, NULL, &cbPointLightPos, 0, 0 ); // 将cbPointLightPos的数据拷贝到mCBPointLightPosBuffer

	//第三,设置在VertexShader的常量缓存的值( 带着更新的值 )
	d3dDeviceContext->VSSetConstantBuffers( 0, 1, &mCBMatrixBuffer ); 

	//第四,设置在PixelShader的常量缓存的值
	d3dDeviceContext->PSSetConstantBuffers( 1, 1, &mCBLightBuffer );
	d3dDeviceContext->PSSetConstantBuffers( 2, 1, &mCBPointLightPosBuffer );

	//第五, 设置在PixelShader的纹理资源
	d3dDeviceContext->PSSetShaderResources( 0, 1, &ColorTexture );
	d3dDeviceContext->PSSetShaderResources( 1, 1, &NormalTexture );
	d3dDeviceContext->PSSetShaderResources( 2, 1, &WposTexture );

	return true;
}

void MultiPointLightShader::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount )
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
XMMATRIX MultiPointLightShader::GetInvenseTranspose( CXMMATRIX ma )
{
	XMMATRIX A = ma;

	//将矩阵A的第四行置零,因为向量是无法进行平移的
	A.r[3] = { 0.0f,0.0f,0.0f,1.0f };

	XMVECTOR det = XMMatrixDeterminant( A );
	XMMATRIX MaInvense = XMMatrixInverse( &det, A );
	return XMMatrixTranspose( MaInvense );
}