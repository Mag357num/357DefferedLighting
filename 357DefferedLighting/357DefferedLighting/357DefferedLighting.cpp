#include"SystemClass.h"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	SystemClass* System = NULL;
	bool result;

	//创建SystemClass类
	System = new SystemClass;

	if ( !System )
	{
		return 0;
	}

	//初始化和运行系统对象
	result = System->Initialize( );
	if ( result )
	{
		System->Run( );
	}

	//关闭systemclass对象
	System->Shutdown( );
	delete System;
	System = NULL;
	return 0;
}