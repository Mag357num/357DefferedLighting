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

	//第一, 填充2D纹理描述, 并创建2D渲染目标纹理
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory( &textureDesc, sizeof( textureDesc ) );

	textureDesc.Width = TextureWidth;
	textureDesc.Height = TexureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //纹理像素为12个字节
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0; 
	textureDesc.MiscFlags = 0;

	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateTexture2D( &textureDesc, NULL, &mRenderTargetTextureArray[i] ) );
	}

	//第二，填充渲染目标视图描述, 并进行创建目标渲染视图
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateRenderTargetView( mRenderTargetTextureArray[i], &renderTargetViewDesc, &mRenderTargetViewArray[i] ) );
	}

	//第三, 填充着色器资源视图描述, 并进行创建着色器资源视图
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		HR( d3dDevice->CreateShaderResourceView( mRenderTargetTextureArray[i], &shaderResourceViewDesc, &mShaderResourceViewArray[i] ) );
	}
	
	//第四, 填充2DTexture深度缓存( 模板缓存 )描述，创建深度缓存( 模板缓存 )
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
	HR( d3dDevice->CreateTexture2D( &depthStencilDesc, //要创建的纹理的描述
		0, 
		&mDepthStencilBuffer ) ); //指向深度缓存的指针

	//第五, 填充深度缓存视图描述, 创建深度缓存( 模板缓存 )视图
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory( &depthStencilViewDesc, sizeof( depthStencilViewDesc ) );
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR( d3dDevice->CreateDepthStencilView( 
		mDepthStencilBuffer, //我们基于这个深度缓存/漏字板缓存创建一个视图
		&depthStencilViewDesc, 
		&mDepthStencilView ) );//指向深度缓存/漏字板视图的指针

	//第六, 设置渲染的视口
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

//让此时所有图形渲染到这个目前渲染的位置
void GeometryBufferClass::SetRenderTargetsAndViewports( ID3D11DeviceContext* deviceContext )
{
	//绑定渲染目标视图和深度模板视图到输出渲染管线，此时渲染输出到两张纹理中
	deviceContext->OMSetRenderTargets( BUFFER_COUNT, mRenderTargetViewArray, mDepthStencilView );

	//设置相应的视口
	deviceContext->RSSetViewports( 1, &md3dViewport );
}

void GeometryBufferClass::ClearRenderTargetView( ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha )
{
	//设置清除缓存为的颜色
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除背后缓存
	for ( int i = 0; i < BUFFER_COUNT; ++i )
	{
		deviceContext->ClearRenderTargetView( mRenderTargetViewArray[i], color );
	}
	
	//清除深度缓存和模板缓存
	deviceContext->ClearDepthStencilView( mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

// 将“被[渲染模型到纹理]的纹理”作为ShaderResourceView资源返回，这个资源将会跟其它的ShaderResourceView资源一样被送入Shader里计算.
ID3D11ShaderResourceView* GeometryBufferClass::GetShaderResourceView( int index )
{
	return mShaderResourceViewArray[index];
}