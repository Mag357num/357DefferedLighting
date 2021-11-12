#include"D3DClass.h"
#include"dxgi.h"

D3DClass::D3DClass( )
{
	md3dDevice = NULL;
	md3dContext = NULL;;
	md3dSwapChain = NULL;
	md3dRenderTargetView = NULL;
	md3dDepthStencilView = NULL;
	md3dDepthStencilBuffer = NULL;
	md3dDepthStencilState = NULL;
	md3dRasterizerState = NULL;
}

D3DClass::~D3DClass( )
{

}

bool D3DClass::Initialize( int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, bool fullscreen, float ScreenDepth, float ScreenNear )
{
	
	float fieldOfView, screenAspect;

	//第一, 获取显示模式信息和显卡信息
	IDXGIFactory* factory;
	IDXGIAdapter* adpter; //适配器
	IDXGIOutput* adapterOutput;
	unsigned int numModes;
	unsigned int numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int result;

	//存储vsyn设定
	mVsyncEnable = vsync;

	//创建一个Directx图形接口factory
	HR( CreateDXGIFactory( __uuidof( IDXGIFactory ), ( void** )&factory ) );

	//使用factory来为显卡创建一个adapter
	HR( factory->EnumAdapters( 0, &adpter ) );

	//列举主要的适配器输出
	HR( adpter->EnumOutputs( 0, &adapterOutput ) );

	//获取适应适配器DXGI_FORMAT_R8G8B8A8_UNORM显示格式的模式数目
	HR( adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL ) );

	//创建一个显示模式列表存放可能的显示模式( 显卡, 监视器 )
	displayModeList = new DXGI_MODE_DESC[numModes];
	if ( !displayModeList )
		return false;

	//填充显示模式列表结构体
	HR( adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList ) );

	//浏览所有的显示模式, 找到适合屏幕宽度和高度的
	//当一个模式匹配, 存储监视器的fps
	for ( int i = 0; i < numModes; i++ )
	{
		if ( displayModeList[i].Width == ( unsigned int )ScreenWidth )
		{
			if ( displayModeList[i].Height == ( unsigned int )ScreenHeight )
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//获取适配器( 显卡 )描述
	HR( adpter->GetDesc( &adapterDesc ) );

	//获取显卡内存量
	mVideoCardMemory = ( int )( adapterDesc.DedicatedVideoMemory / 1024 / 1024 );

	//将显卡名字转存在字符数组
	result = wcstombs_s( &stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128 );
	if ( result != 0 )
	{
		return false;
	}

	//释放显示模式列表
	delete[] displayModeList;
	displayModeList = NULL;
	ReleaseCOM( adpter );
	ReleaseCOM( factory );

	//第二, 填充交换链描述
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );

	sd.BufferDesc.Width = ScreenWidth;
	sd.BufferDesc.Height = ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //使用32位RGBA格式
	if ( mVsyncEnable ) //限不限帧
	{
		sd.BufferDesc.RefreshRate.Numerator = numerator;
		sd.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
	}

	//关闭多重采样MSAA
	sd.SampleDesc.Count = 1; //每像素多重采样个数
	sd.SampleDesc.Quality = 0; //图像质量等级，可选范围为0到ID3D10Device::CheckMultisampleQualityLevels
	
	//是否进行全屏
	if ( fullscreen )
	{
		sd.Windowed = false;
	}
	else
	{
		sd.Windowed = true;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //表示交换链下的后台缓冲去的去处：要将场景渲染到后台缓冲区（即将它用作渲染目标）
	sd.BufferCount = 1; //后台缓存数量
	sd.OutputWindow = hwnd; //交换链所属的窗口, 进行渲染的窗口的句柄
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //交换链下显卡驱动程序选择的显示模式
	sd.Flags = 0; //交换链下的全屏显示模式的可选设置
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //光栅时，扫面的顺序，一般默认
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//第三, 创建交换链和D3D设备和D3D设备上下文
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	HR( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &sd, &md3dSwapChain, &md3dDevice, NULL, &md3dContext ) );

	//第四, 创建后台缓存视图
	ID3D11Texture2D* backBuffer;
	md3dSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
	md3dDevice->CreateRenderTargetView( backBuffer, 0, &md3dRenderTargetView ); //将渲染目标描述变量设为空给我们所有的表面的MIP映射水平都为0级
	ReleaseCOM( backBuffer );

	//第五, 填充2DTexture深度模板缓存描述，创建深度模板缓存
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.Width = ScreenWidth;
	depthStencilDesc.Height = ScreenHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR( md3dDevice->CreateTexture2D( &depthStencilDesc, 0, &md3dDepthStencilBuffer ) );

	//第六, 创建并设定深度模板缓存状态，指示如何使用Depth和stencil( Test )
	D3D11_DEPTH_STENCIL_DESC DSDESC;
	ZeroMemory( &DSDESC, sizeof( DSDESC ) );
	DSDESC.DepthEnable = true;
	DSDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DSDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DSDESC.StencilEnable = true;
	DSDESC.StencilReadMask = 0xff;
	DSDESC.StencilWriteMask = 0xff;
	//前面设定
	DSDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DSDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定, 在光栅化状态剔除背面时这个设定没用, 但是依然要设定, 不然无法创建深度模板状态
	DSDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DSDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DSDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR( md3dDevice->CreateDepthStencilState( &DSDESC, &md3dDepthStencilState ) );

	md3dContext->OMSetDepthStencilState( md3dDepthStencilState, 1 ); //1表示深度模板测试时比对用的参考值

	//第七, 创建一个使ZBuffer无效的DepthStencilState状态
	D3D11_DEPTH_STENCIL_DESC DisableDepthDESC;
	ZeroMemory( &DisableDepthDESC, sizeof( DisableDepthDESC ) );
	DisableDepthDESC.DepthEnable = false;
	DisableDepthDESC.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DisableDepthDESC.DepthFunc = D3D11_COMPARISON_LESS;
	DisableDepthDESC.StencilEnable = true;
	DisableDepthDESC.StencilReadMask = 0xff;
	DisableDepthDESC.StencilWriteMask = 0xff;
	//前面设定
	DisableDepthDESC.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DisableDepthDESC.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//背面设定, 在光栅化状态剔除背面时这个设定没用, 但是依然要设定, 不然无法创建深度( 模板 )状态
	DisableDepthDESC.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DisableDepthDESC.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DisableDepthDESC.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HR( md3dDevice->CreateDepthStencilState( &DisableDepthDESC, &md3dDisableDepthStencilState ) );

	//第八, 创建深度模板缓存视图
	HR( md3dDevice->CreateDepthStencilView( 
		md3dDepthStencilBuffer, 0, &md3dDepthStencilView ) );//指向深度模板视图的指针, 0处为描述

	//第九, 把那些视图绑定到输出合并阶段
	md3dContext->OMSetRenderTargets( 1, &md3dRenderTargetView, md3dDepthStencilView ); //将要绑定的渲染目标的数量, 将要绑定的渲染目标视图数组中的第一个元素的指针, 将要绑定到管线的深度/模板视图

	//第十, 创建并设定光栅化状态, 用于控制如何渲染目标( 以线框还是实体模式等等 )
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false; //是否启用线性抗锯齿，这个选项只有alpha blending启用，画线段并且多重采样关闭的时候才能用
	rasterDesc.CullMode = D3D11_CULL_BACK; //背面剔除
	rasterDesc.DepthBias = 0; //每个像素要添加的深度值
	rasterDesc.DepthBiasClamp = 0.0f; //每个像素最大的深度偏移
	rasterDesc.DepthClipEnable = true; //深度裁剪开启
	rasterDesc.FillMode = D3D11_FILL_SOLID; //实体渲染还是线框
	rasterDesc.FrontCounterClockwise = false; //顺时针
	rasterDesc.MultisampleEnable = false; //是否启用多重采样抗锯齿
	rasterDesc.ScissorEnable = false; //是否启用矩形裁剪，在矩形之外的像素将被裁剪
	rasterDesc.SlopeScaledDepthBias = 0.0f; //指定每个像素的斜率

	HR( md3dDevice->CreateRasterizerState( &rasterDesc, &md3dRasterizerState ) );
	md3dContext->RSSetState( md3dRasterizerState );

	//第十一, 创建并设定视口
	md3dViewport.Width = static_cast<float>( ScreenWidth );
	md3dViewport.Height = static_cast<float>( ScreenHeight );
	md3dViewport.MinDepth = 0.0f;
	md3dViewport.MaxDepth = 1.0f;
	md3dViewport.TopLeftX = 0.0f;
	md3dViewport.TopLeftY = 0.0f;
	md3dContext->RSSetViewports( 1, &md3dViewport );
	
	//第十二, 创建投影矩阵，世界矩阵和用户接口矩阵
	fieldOfView = XM_PIDIV4; //视场角, 四分之pi
	screenAspect = ( float )ScreenWidth / ( float )ScreenHeight;
	mProjMatrix = XMMatrixPerspectiveFovLH( fieldOfView, screenAspect, ScreenNear, ScreenDepth );
	mWorldMatrix = XMMatrixIdentity( );
	mOrthoMatrix = XMMatrixOrthographicLH( static_cast<float>( ScreenWidth ), static_cast<float>( ScreenHeight ), ScreenNear, ScreenDepth ); //2D渲染矩阵, 用于变换UI, 正交投影矩阵

	//第十三, 输出一个Text文件保存显卡的信息 
	char CardInfo[128];
	int memory;
	GetVideoCardInfo( CardInfo, memory );
	ofstream os( "I:/1.txt" );
	os << "memory = " << memory << " " << " CardInfo = " << CardInfo;
	return true;
}

void D3DClass::Shutdown( )
{
	//在释放其它D3D接口前, 使交换链进入窗口模式
	if ( md3dSwapChain )
	{
		md3dSwapChain->SetFullscreenState( false, NULL );
	}

	ReleaseCOM( md3dRenderTargetView );
	ReleaseCOM( md3dDepthStencilView );
	ReleaseCOM( md3dDepthStencilBuffer );
	ReleaseCOM( md3dDepthStencilState );
	ReleaseCOM( md3dRasterizerState );
	ReleaseCOM( md3dSwapChain );
	ReleaseCOM( md3dDevice );
	ReleaseCOM( md3dContext );

}

void D3DClass::BeginScene( float red, float green, float blue, float alpha )
{
	float color[4];

	//设置清除缓存( backbuffer ), 设置每帧初始颜色
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除渲染目标视图
	md3dContext->ClearRenderTargetView( md3dRenderTargetView, color );

	//清除深度模板视图，设置每帧初始值
	md3dContext->ClearDepthStencilView( md3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

void D3DClass::EndScene( )
{
	//因为渲染已经完成, 提交backbuffer到屏幕
	if ( mVsyncEnable )
	{
		//限帧提交
		md3dSwapChain->Present( 1, 0 );
	}
	else
	{
		//尽可能快提交
		md3dSwapChain->Present( 0, 0 );
	}
}

void D3DClass::GetVideoCardInfo( char* cardName, int& memory )
{
	strcpy_s( cardName, 128, mVideoCardDescription );
	memory = mVideoCardMemory;
	return;
}

void D3DClass::SetBackBufferRender( )
{
	md3dContext->OMSetRenderTargets( 1, &md3dRenderTargetView, md3dDepthStencilView );
}

void D3DClass::SetViewPort( )
{
	md3dContext->RSSetViewports( 1, &md3dViewport );
}

void D3DClass::TurnOffZBuffer( )
{
	md3dContext->OMSetDepthStencilState( md3dDisableDepthStencilState, 1 );
}

void D3DClass::TurnOnZBuffer( )
{
	md3dContext->OMSetDepthStencilState( md3dDepthStencilState, 1 );
}