#include"LightClass.h"

LightClass::LightClass( )
{

}

LightClass::~LightClass( )
{

}

XMVECTOR LightClass::GetDiffuseColor( )
{
	return XMLoadFloat4( &mDiffuseColor );
}

XMVECTOR LightClass::GetLightDirection( )
{
	return XMLoadFloat3( &mLightDiretion );
}

void LightClass::SetDiffuseColor( FXMVECTOR diffuseColor )
{
	XMStoreFloat4( &mDiffuseColor, diffuseColor );
}

void LightClass::SetLightDirection( FXMVECTOR lightDirection )
{
	XMStoreFloat3( &mLightDiretion, lightDirection );
}

XMVECTOR LightClass::GetPointLightPostion( )
{
	return XMLoadFloat4( &mPointLightPos );
}

void LightClass::SetPointLightPostion( FXMVECTOR postion )
{
	XMStoreFloat4( &mPointLightPos, postion );
}