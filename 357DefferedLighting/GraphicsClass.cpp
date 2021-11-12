#include"GraphicsClass.h"

GraphicsClass::GraphicsClass( )
{
	//初始化四个类的指针
	mD3D = NULL;
	mCamera = NULL;
	mGeometryBuffer = NULL;
	mMultiLightShader = NULL;
	mDefferredShader = NULL;
	mBitmap = NULL;
	mModel = NULL;

	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		mPoints[i] = NULL;
	}
}

GraphicsClass::~GraphicsClass( )
{

}

bool GraphicsClass::Initialize( int ScreenWidth, int ScreenHeight, HWND hwnd )
{
	bool result;
	
	//第一, 创建D3DClass类并且初始化, D3DClass应该是第一个被创建并且初始化的类, 因为后面的的ColroShaderClass, ModelClass都需要d3dDevice和d3dDeviceContext
	mD3D = new D3DClass( );
	if ( !mD3D )
	{
		return false;
	}
	result = mD3D->Initialize( ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR );
	if ( !result )
	{
		MessageBox( hwnd, L"D3DClass Initialize failure", NULL, MB_OK );
		return false;
	}

	//第二, 创建CameraClass类
	mCamera = new CameraClass( );
	if ( !mCamera )
	{
		return false;
	}
	//初始化相机的位置
	mCamera->SetPostion( 0.0f, 50.0f, -50.0f );
	mCamera->SetRotation( 45.0f, 0.0f, 0.0f );

	//第三, 创建ModelClass并且初始化
	mModel = new ModelClass( );
	if ( !mModel )
	{
		return false;
	}
	result = mModel->Initialize( mD3D->GetDevice( ), ( WCHAR* )L"Texture/seafloor.dds", "Txt/plane01.txt" );
	if ( !result )
	{
		MessageBox( hwnd, L"ModelClass Initialize failure", NULL, MB_OK );
		return false;
	}

	//第四, 创建DefferredShader, 并且进行初始化
	mDefferredShader = new DefferredShaderClass( );
	if ( !mDefferredShader )
	{
		return false;
	}
	result = mDefferredShader->Initialize( mD3D->GetDevice( ), hwnd );
	if ( !result )
	{
		MessageBox( hwnd, L"mDefferredShader Initialize failure", NULL, MB_OK );
		return false;
	}

	//第五, 创建光照shader, 并且进行初始化
	mMultiLightShader = new MultiPointLightShader( );
	if ( !mMultiLightShader )
	{
		return false;
	}
	result = mMultiLightShader->Initialize( mD3D->GetDevice( ), hwnd );
	if ( !result )
	{
		MessageBox( hwnd, L"mMultiLightShader Initialize failure", NULL, MB_OK );
		return false;
	}

	//第六, 创建灯光
	int a = -50, b = 50;
	int c = 0, d = 10;
	srand( ( unsigned )time( NULL ) );
	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		mPoints[i] = new LightClass( );
		if ( !mPoints[i] )
		{
			return false;
		}
		int random1 = ( rand( ) % ( b - a + 1 ) ) + a;
		int random2 = ( rand( ) % ( b - a + 1 ) ) + a;
		int random3 = ( rand( ) % ( d - c + 1 ) ) + c;
		int random4 = ( rand( ) % ( d - c + 1 ) ) + c;
		int random5 = ( rand( ) % ( d - c + 1 ) ) + c;
		mPoints[i]->SetDiffuseColor( XMVectorSet( 0.1f * random3, 0.1f * random4, 0.1f * random5, 1.0f ) );
		mPoints[i]->SetPointLightPostion( XMVectorSet( 1.0f * random1, 1.0f, 1.0f * random2, 1.0f ) );
	}

	//第七, 创建GeometryBuffer( 这其实就是RTT类, 渲染到纹理类 )
	mGeometryBuffer = new GeometryBufferClass( );
	if ( !mGeometryBuffer )
	{
		return false;
	}

	result = mGeometryBuffer->Initialize( mD3D->GetDevice( ), ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR );
	if ( !result )
	{

		MessageBox( hwnd, L"mGeometryBuffer Initialize failure", NULL, MB_OK );
		return false;
	}

	//第八，创建并初始化BitmapClass
	mBitmap = new BitmapClass( );
	if ( !mBitmap )
	{
		return false;
	}

	result = mBitmap->Initialize( mD3D->GetDevice( ), ScreenWidth, ScreenHeight );
	if ( !result )
	{
		MessageBox( hwnd, L"mBitmap Initialize failure", NULL, MB_OK );
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown( )
{
	//释放灯光
	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		if ( mPoints[i] )
		{
			delete mPoints[i];
			mPoints[i] = NULL;
		}
	}

	//释放mCamera
	if ( mCamera )
	{
		delete mCamera;
		mCamera = NULL;
	}

	//释放mModel
	if ( mModel )
	{
		mModel->Shutdown( );
		delete mModel;
		mModel = NULL;
	}

	//释放mMultiLightShader
	if ( mMultiLightShader )
	{
		mMultiLightShader->Shutdown( );
		delete mMultiLightShader;
		mMultiLightShader = NULL;
	}

	//释放mDefferredBuffer
	if ( mGeometryBuffer )
	{
		mGeometryBuffer->ShutDown( );
		delete mGeometryBuffer;
		mGeometryBuffer = NULL;
	}

	//释放BitmapClass
	if ( mBitmap )
	{
		mBitmap->Shutdown( );
		delete mBitmap;
		mBitmap = NULL;
	}

	//释放mDefferredShader
	if ( mDefferredShader )
	{
		mDefferredShader->Shutdown( );
		delete mDefferredShader;
		mDefferredShader = NULL;
	}

	//释放mD3D
	if ( mD3D )
	{
		mD3D->Shutdown( );
		delete mD3D;
		mD3D = NULL;
	}
}

bool GraphicsClass::Frame( )
{
	bool result;
	static float rotation = 0.0f;
	rotation += XM_PI * 0.004f;
	if ( rotation > 360.0f )
		rotation -= 360.0f;
	
	//进行渲染
	result = Render( rotation );
	if ( !result )
		return false;

	return true;
}

bool GraphicsClass::Render( float rotation )
{
	bool result;
	
	//第一, 将3D场景渲染到三张纹理上，一张为颜色纹理，一张为法线纹理( 存储直接的法向量信息 ), 一张为世界坐标
	result = RenderSceneToTexture( rotation );
	if ( !result )
	{
		MessageBox( NULL, L"RenderSceneToTexture failure", NULL, MB_OK );
		return false;
	}

	//第二, 将2D纹理渲染到背后缓存
	result = RenderTextureToBack( rotation );
	if ( !result )
	{
		MessageBox( NULL, L"RenderTextureToBack failure", NULL, MB_OK );
		return false;
	}

	return true;
}

bool GraphicsClass::RenderSceneToTexture( float rotation )
{
	// TODO: 暂停视角旋转
	rotation = 0.0f;

	//三个变换矩阵
	XMMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	bool result;
	//第一, 清除渲染目标视图和深度模板缓存开始绘制场景
	mD3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	//第一，将渲染目标变为Gbuffer对应的纹理
	mGeometryBuffer->SetRenderTargetsAndViewports( mD3D->GetDeviceContext( ) ); // Gbuffer的渲染目标为2D纹理，储存几何信息

	//第二, 清除纹理的缓存
	mGeometryBuffer->ClearRenderTargetView( mD3D->GetDeviceContext( ), 0.0f, 0.0f, 0.0f, 1.0f );

	//第三, ( 更新 )获取ViewMatrix( 根据CameraClass的mPostion和mRotation来生成的 )
	mCamera->PrepareViewMatrix( );

	//第四, 获取三个变换矩阵( WorldMatrix和ProjMatrix来自mD3D类, ViewMatrix来自CameraClass )
	WorldMatrix = mD3D->GetWorldMatrix( );
	ProjMatrix = mD3D->GetProjMatrix( );
	ViewMatrix = mCamera->GetViewMatrix( );

	//顺时针旋转世界矩阵
	WorldMatrix = WorldMatrix * XMMatrixRotationY( rotation );

	//第五, 将3D模型的顶点数据和索引数据放入3D渲染流水线( IA )
	mModel->SetDataToIA( mD3D->GetDeviceContext( ) );

	//第六, 用DefferredShader进行绘制
	result = mDefferredShader->Render( mD3D->GetDeviceContext( ), mModel->GetIndexCount( ), WorldMatrix, ViewMatrix, ProjMatrix, mModel->GetTexture( ) );
	if ( !result )
	{
		MessageBox( NULL, L"mDefferredShaderRender failure", NULL, MB_OK );
		return false;
	}

	return true;
}

bool GraphicsClass::RenderTextureToBack( float rotation )
{
	//三个变换矩阵
	XMMATRIX WorldMatrix, ViewMatrix, OrthoMatrix;
	XMVECTOR PointsDiffseColor[LIGHT_NUM];
	XMVECTOR PointsPos[LIGHT_NUM];
	bool result;

	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		PointsDiffseColor[i] = XMVector3TransformCoord( mPoints[i]->GetDiffuseColor( ), XMMatrixRotationY( rotation ) );
		PointsPos[i] = XMVector3TransformCoord( mPoints[i]->GetPointLightPostion( ), XMMatrixRotationY( rotation / 2 ) );
	}

	//第一, 清除缓存开始绘制场景
	mD3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	//第二, 重置设置渲染目标为背后缓存
	mD3D->SetBackBufferRender( ); // d3d的渲染目标为交换链

	//第三，重新设置视口
	mD3D->SetViewPort( );

	//第四, 生成基础相机变换矩阵
	mCamera->PrepareViewMatrix( );

	//第五, 获取三个变换矩阵( WorldMatrix和ProjMatrix来自mD3D类, ViewMatrix来自CameraClass )
	WorldMatrix = mD3D->GetWorldMatrix( );
	OrthoMatrix = mD3D->GetOrthoMatrix( );
	ViewMatrix = mCamera->GetBaseViewMatrix( ); // 由于关于视角的变换已经在DeferredLightShader做过了, 因此这里只要取正交变换

	//第六，关闭深度模板测试
	mD3D->TurnOffZBuffer( );

	//第七, 把2D图片的数据放入3D渲染流水线( IA )
	mBitmap->SetDataToIA( mD3D->GetDeviceContext( ) );

	//第八，用mMultiLightShader进行渲染
	result = mMultiLightShader->Render( mD3D->GetDeviceContext( ), mBitmap->GetIndexCount( ), WorldMatrix, ViewMatrix, OrthoMatrix,
		mGeometryBuffer->GetShaderResourceView( 0 ), mGeometryBuffer->GetShaderResourceView( 1 ), mGeometryBuffer->GetShaderResourceView( 2 ), PointsDiffseColor, PointsPos );
	if ( !result )
	{
		MessageBox( NULL, L"mMultiLightShader failure", NULL, MB_OK );
		return false;
	}

	//第九，打开深度模板测试
	mD3D->TurnOnZBuffer( );
 
	//把渲染的场景呈献给屏幕
	mD3D->EndScene( );

	return true;
}