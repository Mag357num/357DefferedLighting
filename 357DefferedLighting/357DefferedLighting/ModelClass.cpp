#include"ModelClass.h"

ModelClass::ModelClass( )
{
	md3dVertexBuffer = NULL; //顶点缓存
	md3dIndexBuffer = NULL; //索引缓存
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

	//第一，初始化外部模型数据, 这个发生在初始化缓存之前，因为顶点缓存索引缓存来自外部模型数据
	result = LoadModelData( ModelFileName );
	if ( !result )
	{
		MessageBox( NULL, L"Load ModelData failure", L"ERROR", MB_OK );
		return false;
	}

	//第二, 初始化顶点缓存，索引缓存
	result = InitializeBuffer( d3dDevice );
	if ( !result )
	{
		MessageBox( NULL, L"Initialize Buffer failure", L"ERROR", MB_OK );
		return false;
	}
		
	//第三, 加载纹理资源
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
	//设置渲染管线的顶点缓存和索引缓存( IA阶段 )
	SetBuffersToIA( d3dDeviceContext );
}

bool ModelClass::InitializeBuffer( ID3D11Device* d3dDevice )
{
	Vertex* vertexs = NULL;
	WORD* indices = NULL; //一个WORD两个字节

	//创建顶点数组
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
		return false;

	//创建索引数组
	indices = new WORD[mIndexCount];
	if ( !indices )
		return false;
	
	//赋予顶点数组数据和索引数组数据
	for ( int i = 0; i < mVertexCount; ++i )
	{
		vertexs[i].pos = XMFLOAT3( mModelData[i].x, mModelData[i].y, mModelData[i].z );
		vertexs[i].tex = XMFLOAT2( mModelData[i].u, mModelData[i].v );
		vertexs[i].normal = XMFLOAT3( mModelData[i].nx, mModelData[i].ny, mModelData[i].nz );
	}
	//赋予索引数组数据
	for ( int i = 0; i < mIndexCount; ++i )
	{
		indices[i] = i;
	}

	//第一, 填充顶点缓存描述和子资源数据结构体, 并创建顶点缓存
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

	//第二, 填充( 索引 )缓存描述和子资源数据结构体, 并创建索引缓存
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

	//释放顶点数组和索引数组( 这时数据已经载入缓存, 不需要这些数组了 )
	delete[]vertexs;
	vertexs = NULL;
	delete[]indices;
	indices = NULL;
	
	return true;
}

void ModelClass::ShutdownBuffer( )
{
	//释放顶点缓存和索引缓存
	ReleaseCOM( md3dIndexBuffer );
	ReleaseCOM( md3dVertexBuffer );

	//释放mTexClass的资源
	ReleaseTexture( );

}

void ModelClass::SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext )
{
	//设置顶点缓存
	UINT stride = sizeof( Vertex ); //每个顶点元素的跨度大小，或者说每个顶点元素的大小
	UINT offset = 0;
	d3dDeviceContext->IASetVertexBuffers( 0, 1, &md3dVertexBuffer, &stride, &offset );

	//设置索引缓存
	d3dDeviceContext->IASetIndexBuffer( md3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0 ); //Word为两个字节

	//设置拓扑方式
	d3dDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

bool ModelClass::LoadTexture( ID3D11Device* d3dDevice, WCHAR* TexFileName )
{
	bool result;

	mTexClass = new TexClass( );
	if ( !mTexClass )
	{
		MessageBox( NULL, L"ITexClass 分配内存 failure", L"ERROR", MB_OK );
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

	//line代表文件里的一整行, 包含空格，字符等。 而word代表文件里的一个单词，无空格存在
	string line, word;
	
	//首先在第一行读取出顶点数( 这里顶点数每三个就是一个三角形 )
	getline( in, line );
	istringstream record( line );
	for ( int i = 0; i < 3; ++i )
	{
		record >> word;
	}

	//这种文件格式下顶点数量和索引数量是一样的
	mVertexCount = mIndexCount = atoi( &word[0] ); //atoi: const char*转int

	mModelData = new ModelType[mVertexCount];
	if ( !mModelData )
	{
		return false;
	}

	//跳出“Data:”这一行
	getline( in, line );

	//读取真正的顶点数据
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

	//TODO: 临时用的缩放
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