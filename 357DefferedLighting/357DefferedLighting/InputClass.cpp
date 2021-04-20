#include"InputClass.h"

InputClass::InputClass( )
{

}

InputClass::~InputClass( )
{

}

void InputClass::Initialize( )
{
	for ( int i = 0; i < 256; ++i )
	{
		mKey[i] = false;
	}
}

void InputClass::KeyDown( unsigned int input )
{
	mKey[input] = true;
}

void InputClass::KeyUp( unsigned int input )
{
	mKey[input] = false;
}

bool InputClass::IsKeyDown( unsigned int key )
{
	return mKey[key];
}