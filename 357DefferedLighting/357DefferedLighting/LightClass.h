#pragma once
#ifndef _LIGHT_CLASS
#define _LIGHT_CLASS

#include<Windows.h>
#include <xnamath.h>

class LightClass
{
private:
	XMFLOAT4	mPointLightPos;
	XMFLOAT4	mDiffuseColor;
	XMFLOAT3	mLightDiretion;

public:
	LightClass( );
	~LightClass( );

public:
	void SetDiffuseColor( FXMVECTOR diffuseColor );
	void SetLightDirection( FXMVECTOR lightDirection );
	void SetPointLightPostion( FXMVECTOR PointLightPos );
	XMVECTOR GetDiffuseColor( );
	XMVECTOR GetLightDirection( );
	XMVECTOR GetPointLightPostion( );
};
#endif // !_LIGHT_CLASS