#include"GeometryBufferClass.h"


GeometryBufferClass::GeometryBufferClass( )
{
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		mRenderTargetTextureArray[i] = NULL;
		mRenderTargetViewArray[i] = NULL;
		mShaderResourceViewArray[i] = NULL;
	}
	mDepthStencilBuffer = NULL;
	mDepthStencilView = NULL;
}

GeometryBufferClass::~GeometryBufferClass( )
{

}

bool GeometryBufferClass::Initialize( ID3D11Device* d3dDevice, int TextureWidth, int TexureHeight, float ScreenDepth, float ScreenNear )
{

	//��һ, ���2D��������, ������2D��ȾĿ������
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory( &textureDesc, sizeof( textureDesc ) );

	textureDesc.Width = TextureWidth;
	textureDesc.Height = TexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //��������Ϊ12���ֽ�
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;

	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateTexture2D( &textureDesc, NULL, &mRenderTargetTextureArray[i] ) );
	}

	//�ڶ��������ȾĿ����ͼ����, �����д���Ŀ����Ⱦ��ͼ
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateRenderTargetView( mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i] ) );
	}

	//����, �����ɫ����Դ��ͼ����, �����д�����ɫ����Դ��ͼ
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateShaderResourceView( mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i] ) );
	}
	
	//����, ���2DTexture��Ȼ���( ģ�建�� )������������Ȼ���( ģ�建�� )
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.Width = TextureWidth;
	depthStencilDesc.Height = TexureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR( d3dDevice->CreateTexture2D( &depthStencilDesc, //Ҫ���������������
		0, 
		&mDepthStencilBuffer ) ); //ָ����Ȼ����ָ��

	//����, �����Ȼ�����ͼ����, ������Ȼ���( ģ�建�� )��ͼ
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR( d3dDevice->CreateDepthStencilView( 
		mDepthStencilBuffer, //���ǻ��������Ȼ���/©�ְ建�洴��һ����ͼ
		&depthStencilViewDesc, 
		&mDepthStencilView ) );//ָ����Ȼ���/©�ְ���ͼ��ָ��

	//����, ������Ⱦ���ӿ�
	md3dViewport.Width = static_cast<float>( TextureWidth );
	md3dViewport.Height = static_cast<float>( TexureHeight );
	md3dViewport.MinDepth = 0.0f;
	md3dViewport.MaxDepth = 1.0f;
	md3dViewport.TopLeftX = 0.0f;
	md3dViewport.TopLeftY = 0.0f;

	return true;
}

void GeometryBufferClass::ShutDown( )
{
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		ReleaseCOM( mShaderResourceViewArray[i] );
		ReleaseCOM( mRenderTargetTextureArray[i] );
		ReleaseCOM( mRenderTargetViewArray[i] );
	}
	ReleaseCOM( mDepthStencilBuffer );
	ReleaseCOM( mDepthStencilView );
}

//�ô�ʱ����ͼ����Ⱦ�����Ŀǰ��Ⱦ��λ��
void GeometryBufferClass::SetRenderTargetsAndViewports( ID3D11DeviceContext* deviceContext )
{
	//����ȾĿ����ͼ�����ģ����ͼ�������Ⱦ���ߣ���ʱ��Ⱦ���������������
	deviceContext->OMSetRenderTargets( BUFFER_COUNT, mRenderTargetViewArray, mDepthStencilView );

	//������Ӧ���ӿ�
	deviceContext->RSSetViewports( 1, &md3dViewport );
}

void GeometryBufferClass::ClearRenderTargetView( ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha )
{
	//�����������Ϊ����ɫ
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//������󻺴�
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		deviceContext->ClearRenderTargetView( mRenderTargetViewArray[i], color );
	}
	
	//�����Ȼ����ģ�建��
	deviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

// ������[��Ⱦģ�͵�����]��������ΪShaderResourceView��Դ���أ������Դ�����������ShaderResourceView��Դһ��������Shader�����.
ID3D11ShaderResourceView* GeometryBufferClass::GetShaderResourceView( int index )
{
	return mShaderResourceViewArray[index];
}