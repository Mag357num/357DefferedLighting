#include"TexClass.h"

TexClass::TexClass( )
{
	mTexture = NULL;
}

TexClass::TexClass( const TexClass&other )
{

}

TexClass::~TexClass( )
{

}

bool TexClass::Initilize( ID3D11Device* d3dDevice, WCHAR* TextureFilename )
{
	HR( D3DX11CreateShaderResourceViewFromFileW( d3dDevice, TextureFilename, NULL, NULL, &mTexture, NULL ) );
	return true;
}

void TexClass::Shutdown( )
{
	ReleaseCOM( mTexture );
}

ID3D11ShaderResourceView* TexClass::GetTexture( )
{
	return mTexture;
}