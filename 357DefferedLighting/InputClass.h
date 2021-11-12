#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H
class InputClass
{
private:
	//��Ӧ�ļ��Ƿ񱻰���
	bool mKey[256];

public:
	InputClass( );
	~InputClass( );

public:
	void Initialize( );
	void KeyDown( unsigned int wParam );
	void KeyUp( unsigned int wParam );
	bool IsKeyDown( unsigned int wParam );
};
#endif