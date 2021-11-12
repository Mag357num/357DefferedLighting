#pragma once
#ifndef _SYSTEM_CLASS_H
#define _SYSTEM_CLASS_H
#define WIN32_LEAN_AND_MEAN	//�������Լ���Win32ͷ�ļ��Ĵ�С

#include"InputClass.h"
#include<Windows.h>
#include"GraphicsClass.h"

class SystemClass
{

private:
	LPCWSTR			mApplicationName;				//Ӧ������
	HINSTANCE		mHinstance;						//Ӧ��ʵ�����
	HWND			mHwnd;							//Ӧ�ô��ھ��
	InputClass*		m_Input;						//������
	GraphicsClass*	m_Graphics;						//ͼ����

	bool			Frame( );						//֡����
	void			InitializeWindow( int&, int& );	//��ʼ�����ں���
	void			ShutdownWindow( );				//�رմ��ں���

public:
	SystemClass( );
	~SystemClass( );
	bool Initialize( );
	void Shutdown( );
	void Run( );

	/*��Ϣ��������, ��������������ܵ���Ϣ�ܶഫ�ݸ�ȫ�־�̬����WndProc����, ����ص�������Щ����, ��ע��*/
	LRESULT CALLBACK MessageHandler( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
};
#endif