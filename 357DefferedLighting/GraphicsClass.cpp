#include"GraphicsClass.h"

GraphicsClass::GraphicsClass( )
{
	//��ʼ���ĸ����ָ��
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
	
	//��һ, ����D3DClass�ಢ�ҳ�ʼ��, D3DClassӦ���ǵ�һ�����������ҳ�ʼ������, ��Ϊ����ĵ�ColroShaderClass, ModelClass����Ҫd3dDevice��d3dDeviceContext
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

	//�ڶ�, ����CameraClass��
	mCamera = new CameraClass( );
	if ( !mCamera )
	{
		return false;
	}
	//��ʼ�������λ��
	mCamera->SetPostion( 0.0f, 50.0f, -50.0f );
	mCamera->SetRotation( 45.0f, 0.0f, 0.0f );

	//����, ����ModelClass���ҳ�ʼ��
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

	//����, ����DefferredShader, ���ҽ��г�ʼ��
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

	//����, ��������shader, ���ҽ��г�ʼ��
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

	//����, �����ƹ�
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

	//����, ����GeometryBuffer( ����ʵ����RTT��, ��Ⱦ�������� )
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

	//�ڰˣ���������ʼ��BitmapClass
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
	//�ͷŵƹ�
	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		if ( mPoints[i] )
		{
			delete mPoints[i];
			mPoints[i] = NULL;
		}
	}

	//�ͷ�mCamera
	if ( mCamera )
	{
		delete mCamera;
		mCamera = NULL;
	}

	//�ͷ�mModel
	if ( mModel )
	{
		mModel->Shutdown( );
		delete mModel;
		mModel = NULL;
	}

	//�ͷ�mMultiLightShader
	if ( mMultiLightShader )
	{
		mMultiLightShader->Shutdown( );
		delete mMultiLightShader;
		mMultiLightShader = NULL;
	}

	//�ͷ�mDefferredBuffer
	if ( mGeometryBuffer )
	{
		mGeometryBuffer->ShutDown( );
		delete mGeometryBuffer;
		mGeometryBuffer = NULL;
	}

	//�ͷ�BitmapClass
	if ( mBitmap )
	{
		mBitmap->Shutdown( );
		delete mBitmap;
		mBitmap = NULL;
	}

	//�ͷ�mDefferredShader
	if ( mDefferredShader )
	{
		mDefferredShader->Shutdown( );
		delete mDefferredShader;
		mDefferredShader = NULL;
	}

	//�ͷ�mD3D
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
	
	//������Ⱦ
	result = Render( rotation );
	if ( !result )
		return false;

	return true;
}

bool GraphicsClass::Render( float rotation )
{
	bool result;
	
	//��һ, ��3D������Ⱦ�����������ϣ�һ��Ϊ��ɫ����һ��Ϊ��������( �洢ֱ�ӵķ�������Ϣ ), һ��Ϊ��������
	result = RenderSceneToTexture( rotation );
	if ( !result )
	{
		MessageBox( NULL, L"RenderSceneToTexture failure", NULL, MB_OK );
		return false;
	}

	//�ڶ�, ��2D������Ⱦ�����󻺴�
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
	// TODO: ��ͣ�ӽ���ת
	rotation = 0.0f;

	//�����任����
	XMMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	bool result;
	//��һ, �����ȾĿ����ͼ�����ģ�建�濪ʼ���Ƴ���
	mD3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	//��һ������ȾĿ���ΪGbuffer��Ӧ������
	mGeometryBuffer->SetRenderTargetsAndViewports( mD3D->GetDeviceContext( ) ); // Gbuffer����ȾĿ��Ϊ2D�������漸����Ϣ

	//�ڶ�, �������Ļ���
	mGeometryBuffer->ClearRenderTargetView( mD3D->GetDeviceContext( ), 0.0f, 0.0f, 0.0f, 1.0f );

	//����, ( ���� )��ȡViewMatrix( ����CameraClass��mPostion��mRotation�����ɵ� )
	mCamera->PrepareViewMatrix( );

	//����, ��ȡ�����任����( WorldMatrix��ProjMatrix����mD3D��, ViewMatrix����CameraClass )
	WorldMatrix = mD3D->GetWorldMatrix( );
	ProjMatrix = mD3D->GetProjMatrix( );
	ViewMatrix = mCamera->GetViewMatrix( );

	//˳ʱ����ת�������
	WorldMatrix = WorldMatrix * XMMatrixRotationY( rotation );

	//����, ��3Dģ�͵Ķ������ݺ��������ݷ���3D��Ⱦ��ˮ��( IA )
	mModel->SetDataToIA( mD3D->GetDeviceContext( ) );

	//����, ��DefferredShader���л���
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
	//�����任����
	XMMATRIX WorldMatrix, ViewMatrix, OrthoMatrix;
	XMVECTOR PointsDiffseColor[LIGHT_NUM];
	XMVECTOR PointsPos[LIGHT_NUM];
	bool result;

	for (size_t i = 0; i < LIGHT_NUM; i++)
	{
		PointsDiffseColor[i] = XMVector3TransformCoord( mPoints[i]->GetDiffuseColor( ), XMMatrixRotationY( rotation ) );
		PointsPos[i] = XMVector3TransformCoord( mPoints[i]->GetPointLightPostion( ), XMMatrixRotationY( rotation / 2 ) );
	}

	//��һ, ������濪ʼ���Ƴ���
	mD3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	//�ڶ�, ����������ȾĿ��Ϊ���󻺴�
	mD3D->SetBackBufferRender( ); // d3d����ȾĿ��Ϊ������

	//���������������ӿ�
	mD3D->SetViewPort( );

	//����, ���ɻ�������任����
	mCamera->PrepareViewMatrix( );

	//����, ��ȡ�����任����( WorldMatrix��ProjMatrix����mD3D��, ViewMatrix����CameraClass )
	WorldMatrix = mD3D->GetWorldMatrix( );
	OrthoMatrix = mD3D->GetOrthoMatrix( );
	ViewMatrix = mCamera->GetBaseViewMatrix( ); // ���ڹ����ӽǵı任�Ѿ���DeferredLightShader������, �������ֻҪȡ�����任

	//�������ر����ģ�����
	mD3D->TurnOffZBuffer( );

	//����, ��2DͼƬ�����ݷ���3D��Ⱦ��ˮ��( IA )
	mBitmap->SetDataToIA( mD3D->GetDeviceContext( ) );

	//�ڰˣ���mMultiLightShader������Ⱦ
	result = mMultiLightShader->Render( mD3D->GetDeviceContext( ), mBitmap->GetIndexCount( ), WorldMatrix, ViewMatrix, OrthoMatrix,
		mGeometryBuffer->GetShaderResourceView( 0 ), mGeometryBuffer->GetShaderResourceView( 1 ), mGeometryBuffer->GetShaderResourceView( 2 ), PointsDiffseColor, PointsPos );
	if ( !result )
	{
		MessageBox( NULL, L"mMultiLightShader failure", NULL, MB_OK );
		return false;
	}

	//�ھţ������ģ�����
	mD3D->TurnOnZBuffer( );
 
	//����Ⱦ�ĳ������׸���Ļ
	mD3D->EndScene( );

	return true;
}