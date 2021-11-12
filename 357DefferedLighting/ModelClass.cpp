#include"ModelClass.h"

ModelClass::ModelClass( )
{
	md3dVertexBuffer = NULL; //���㻺��
	md3dIndexBuffer = NULL; //��������
	mTexClass = NULL;
	mModelData = NULL;
	mVertexCount = 0;
	mIndexCount = 0;
}

ModelClass::~ModelClass( )
{

}

bool ModelClass::Initialize( ID3D11Device* d3dDevice, WCHAR* TexFileName, string ModelFileName )
{
	bool result;

	//��һ����ʼ���ⲿģ������, ��������ڳ�ʼ������֮ǰ����Ϊ���㻺���������������ⲿģ������
	result = LoadModelData( ModelFileName );
	if ( !result )
	{
		MessageBox( NULL, L"Load ModelData failure", L"ERROR", MB_OK );
		return false;
	}

	//�ڶ�, ��ʼ�����㻺�棬��������
	result = InitializeBuffer( d3dDevice );
	if ( !result )
	{
		MessageBox( NULL, L"Initialize Buffer failure", L"ERROR", MB_OK );
		return false;
	}
		
	//����, ����������Դ
	result = LoadTexture( d3dDevice, TexFileName );
	if ( !result )
	{
		MessageBox( NULL, L"Load Texture failure", L"ERROR", MB_OK );
		return false;
	}
	
	return true;
}

void ModelClass::Shutdown( )
{
	ShutdownBuffer( );
	ReleaseTexture( );
	ReleaseModelData( );
}


void ModelClass::SetDataToIA( ID3D11DeviceContext* d3dDeviceContext )
{
	//������Ⱦ���ߵĶ��㻺�����������( IA�׶� )
	SetBuffersToIA( d3dDeviceContext );
}

bool ModelClass::InitializeBuffer( ID3D11Device* d3dDevice )
{
	Vertex* vertexs = NULL;
	WORD* indices = NULL; //һ��WORD�����ֽ�

	//������������
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
		return false;

	//������������
	indices = new WORD[mIndexCount];
	if ( !indices )
		return false;
	
	//���趥���������ݺ�������������
	for ( int i = 0; i < mVertexCount; ++i )
	{
		vertexs[i].pos = XMFLOAT3( mModelData[i].x, mModelData[i].y, mModelData[i].z );
		vertexs[i].tex = XMFLOAT2( mModelData[i].u, mModelData[i].v );
		vertexs[i].normal = XMFLOAT3( mModelData[i].nx, mModelData[i].ny, mModelData[i].nz );
	}
	//����������������
	for ( int i = 0; i < mIndexCount; ++i )
	{
		indices[i] = i;
	}

	//��һ, ��䶥�㻺������������Դ���ݽṹ��, ���������㻺��
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertexs;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HR( d3dDevice->CreateBuffer( &vertexBufferDesc, &vertexData, &md3dVertexBuffer ) );

	//�ڶ�, ���( ���� )��������������Դ���ݽṹ��, ��������������
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof( WORD ) * mIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	HR( d3dDevice->CreateBuffer( &indexBufferDesc, &indexData, &md3dIndexBuffer ) );

	//�ͷŶ����������������( ��ʱ�����Ѿ����뻺��, ����Ҫ��Щ������ )
	delete[]vertexs;
	vertexs = NULL;
	delete[]indices;
	indices = NULL;
	
	return true;
}

void ModelClass::ShutdownBuffer( )
{
	//�ͷŶ��㻺�����������
	ReleaseCOM( md3dIndexBuffer );
	ReleaseCOM( md3dVertexBuffer );

	//�ͷ�mTexClass����Դ
	ReleaseTexture( );

}

void ModelClass::SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext )
{
	//���ö��㻺��
	UINT stride = sizeof( Vertex ); //ÿ������Ԫ�صĿ�ȴ�С������˵ÿ������Ԫ�صĴ�С
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers( 0, 1, &md3dVertexBuffer, &stride, &offset );

	//������������
	d3dDeviceContext->IASetIndexBuffer( md3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0 ); //WordΪ�����ֽ�

	//�������˷�ʽ
	d3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

bool ModelClass::LoadTexture( ID3D11Device* d3dDevice, WCHAR* TexFileName )
{
	bool result;

	mTexClass = new TexClass( );
	if ( !mTexClass )
	{
		MessageBox( NULL, L"ITexClass �����ڴ� failure", L"ERROR", MB_OK );
		return false;
	}
	result = mTexClass->Initilize( d3dDevice, TexFileName );

	if ( !result )
	{
		return false;
	}
	return true;

}

void ModelClass::ReleaseTexture( )
{
	if ( mTexClass )
	{
		mTexClass->Shutdown( );
		delete mTexClass;
		mTexClass = NULL;
	}

}

void ModelClass::ReleaseModelData( )
{

	if ( mModelData )
	{
		delete mModelData;
		mModelData = NULL;
	}
}

bool ModelClass::LoadModelData( string ModelFileName )
{
	ifstream in( ModelFileName );

	//line�����ļ����һ����, �����ո��ַ��ȡ� ��word�����ļ����һ�����ʣ��޿ո����
	string line, word;
	
	//�����ڵ�һ�ж�ȡ��������( ���ﶥ����ÿ��������һ�������� )
	getline( in, line );
	istringstream record( line );
	for ( int i = 0; i < 3; ++i )
	{
		record >> word;
	}

	//�����ļ���ʽ�¶�������������������һ����
	mVertexCount = mIndexCount = atoi( &word[0] ); //atoi: const char*תint

	mModelData = new ModelType[mVertexCount];
	if ( !mModelData )
	{
		return false;
	}

	//������Data:����һ��
	getline( in, line );

	//��ȡ�����Ķ�������
	for ( int i = 0; i < mVertexCount; ++i )
	{
		getline( in, line );
		istringstream record( line );

		//x
		record >> word;
		mModelData[i].x = atof( &word[0] );
		//y
		record >> word;
		mModelData[i].y = atof( &word[0] );
		//z
		record >> word;
		mModelData[i].z = atof( &word[0] );
		//u
		record >> word;
		mModelData[i].u = atof( &word[0] );
		//v
		record >> word;
		mModelData[i].v = atof( &word[0] );
		//nx
		record >> word;
		mModelData[i].nx = atof( &word[0] );
		//ny
		record >> word;
		mModelData[i].ny = atof( &word[0] );
		//nz
		record >> word;
		mModelData[i].nz = atof( &word[0] );
	}

	//TODO: ��ʱ�õ�����
	for ( int i = 0; i < mVertexCount; ++i )
	{
		mModelData[i].x *= 10;
		mModelData[i].y *= 10;
		mModelData[i].z *= 10;
		mModelData[i].u *= 10;
		mModelData[i].v *= 10;
	}

	return true;
}