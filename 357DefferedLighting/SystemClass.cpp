#include"SystemClass.h"

//ȫ�ֱ���
static SystemClass* _d3dApp = NULL;

//����SystemClass������ȫ�ֻص�����
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_DESTROY:
	{
		PostQuitMessage( 0 ); //����WM_QUIT��Ϣ����Ϣ���� ���յ�WM_QUIT������Ϣѭ��
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage( 0 );
		return 0;
	}

	//��������Ϣ���͵�D3DAPPҲ����SystemClass�����MessageHandler����
	default:
		return _d3dApp->MessageHandler( hwnd, message, wParam, lParam );

	}
}

SystemClass::SystemClass( )
{
	m_Graphics = NULL;
	m_Input = NULL;
}

/*�������������л����ڴ��һ��ԭ���ǽ����Դ��ڴ�Ļ�������*/
SystemClass::~SystemClass( )
{

}

/*ϵͳ���ʼ������*/
bool SystemClass::Initialize( )
{
	int ScreenWidth, ScreenHeight;
	bool result;

	/*��ʼ����Ļ��Ⱥ͸߶�*/
	ScreenWidth = 0;
	ScreenHeight = 0;

	/*��ʼ��ϵͳ��Ĵ���*/
	InitializeWindow( ScreenWidth, ScreenHeight );

	/*�������������*/
	m_Input = new InputClass( );
	if ( !m_Input )
	{
		return false;
	}

	/*��ʼ���������*/
	m_Input->Initialize( );

	/*����ͼ�������*/
	m_Graphics = new GraphicsClass( );
	if ( !m_Graphics )
	{
		return false;
	}

	result = m_Graphics->Initialize( ScreenWidth, ScreenHeight, mHwnd );
	if ( !result )
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown( )
{
	/*�ͷ�ͼ�������*/
	if ( m_Graphics )
	{
		m_Graphics->Shutdown( );
		delete m_Graphics;
		m_Graphics = NULL;
	}

	/*�ͷ����������*/
	if ( m_Input )
	{
		delete m_Input;
		m_Input = NULL;
	}

	/*�رմ���*/
	this->ShutdownWindow( );
}

void SystemClass::Run( )
{
	MSG msg = { 0 };
	bool done, result;

	/*ѭ��ֱ���յ����Դ��ڵĻ���ʹ���ߵ�quit��Ϣ*/
	done = false;
	while ( !done )
	{
		//����������Ϣ
		if ( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
			{
				done = true;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg ); //����Ϣ���͵�WindProc( )��
		}
		else
		{
			result = Frame( ); //Frame���еĺ������������Ϸ�˳�
			if ( !result )
			{
				done = true;
			}
		}
	}
}

bool SystemClass::Frame( )
{
	bool result;

	//�㰴�����˳�����, ���˳����Ӧ��( �����ĺ�պ����Ӧ )
	if ( m_Input->IsKeyDown( VK_ESCAPE ) )
	{
		return false;
	}
	
	//����ͼ�ζ����֡����
	result = m_Graphics->Frame( );
	if ( !result )
	{
		return false;
	}
	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{

	 //ȷ��һ�����Ѿ��ڼ����ϰ���
	 case WM_KEYDOWN:
	 {
		 //���һ����������, �����������Ϣ�͵�input object, Ϊ�˿��Լ�¼���״̬
		 m_Input->KeyDown( ( unsigned int )wParam );
		 return 0;
	 }

	 //ȷ��һ�������ͷ�
	 case WM_KEYUP:
	 {
		 //���һ�������ͷ�, �����������Ϣ�͵�input object, Ϊ�˿���ȡ�����״̬
		 m_Input->KeyUp( ( unsigned int )wParam );
		 return 0;
	 }

	 //��������Ϣ���͵�Ĭ�ϴ�����Ϣ����
	 default:
	 {
		 return DefWindowProc( hwnd, message, wParam, lParam ); //Ϊȫ�־�̬����
	 }
	}
}

void SystemClass::InitializeWindow( int& ScrrenWidth, int &ScrrenHeight )
{
	WNDCLASSEX wc; //������
	DEVMODE dmScrrenSettings;
	int posX, posY;

	//��ȡһ�������ָ����������ָ��
	_d3dApp = this;

	//��ȡӦ��ʵ�����
	mHinstance = GetModuleHandle( NULL );

	//����Ӧ��һ������
	mApplicationName = L"357DefferedLighint";

	//�趨Ҫ�������������	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHinstance;
	wc.hIcon = LoadIcon( NULL, IDI_WINLOGO );
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mApplicationName;
	wc.cbSize = sizeof( WNDCLASSEX );

	//ע�������
	RegisterClassEx( &wc );

	//��ȡ��Ļ�ֱ��ʵĿ�Ⱥ͸߶�
	ScrrenWidth = GetSystemMetrics( SM_CXSCREEN );
	ScrrenHeight = GetSystemMetrics( SM_CYSCREEN );

	//ȡ�����Ƿ�Ϊȫ��Ļ
	if ( FULL_SCREEN )
	{
		//���Ϊȫ��Ļ, ���趨��ĻΪ�û��������󻯲���Ϊ32bit
		memset( &dmScrrenSettings, 0, sizeof( dmScrrenSettings ) );
		dmScrrenSettings.dmSize = sizeof( dmScrrenSettings );
		dmScrrenSettings.dmPelsWidth = ( unsigned long )ScrrenWidth;
		dmScrrenSettings.dmPelsHeight = ( unsigned long )ScrrenHeight;
		dmScrrenSettings.dmBitsPerPel = 32;
		dmScrrenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//�ı���ʾ�趨, ����Ϊȫ��Ļ
		ChangeDisplaySettings( &dmScrrenSettings, CDS_FULLSCREEN );

		//�趨�������Ͻǵ�λ��
		posX = posY = 0;
	}
	else
	{
		//������ڻ�, �趨Ϊ1280 * 960�ֱ���
		ScrrenWidth = 1280;
		ScrrenHeight = 960;

		//����λ����Ļ��΢�����Ϸ�
		posX = ( GetSystemMetrics( SM_CXSCREEN ) - ScrrenWidth ) / 2-200;
		posY = ( GetSystemMetrics( SM_CYSCREEN ) - ScrrenHeight ) / 2-100;

	}

	//��������, ���һ�ȡ���ڵľ��
	mHwnd = CreateWindowEx( WS_EX_APPWINDOW, mApplicationName, mApplicationName, 
		WS_OVERLAPPEDWINDOW, 
		posX, posY, ScrrenWidth, ScrrenHeight, NULL, NULL, mHinstance, NULL );

	//��������ʾ����Ļ֮��, ���趨�ô���Ϊ��Ҫ���е�
	ShowWindow( mHwnd, SW_SHOW );
	SetForegroundWindow( mHwnd );
	SetFocus( mHwnd );
	ShowCursor( true );
}

void SystemClass::ShutdownWindow( )
{
	//��ʾ�����
	//ShowCursor( true );

	//����뿪ȫ��Ļģʽ, �ָ���ʾ�趨
	if ( FULL_SCREEN )
	{
		ChangeDisplaySettings( NULL, 0 );
	}

	//�Ƴ�( �ƻ� )����
	DestroyWindow( mHwnd );
	mHwnd = NULL;

	//�Ƴ�����ʵ��
	UnregisterClass( mApplicationName, mHinstance );
	mHinstance = NULL;

	//�ÿ�Ӧ�������
	_d3dApp = NULL;
}