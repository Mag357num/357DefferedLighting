#pragma once
#ifndef _INPUT_CLASS_H
#define _INPUT_CLASS_H
class InputClass
{
private:
	//对应的键是否被按下
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