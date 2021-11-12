#include"BitmapClass.h"

BitmapClass::BitmapClass( )
{
	md3dVertexBuffer = NULL; //顶点缓存
	md3dIndexBuffer = NULL; //索引缓存
	mVertexCount = 0;
	mIndexCount = 0;
}

BitmapClass::~BitmapClass( )
{

}

bool BitmapClass::Initialize( ID3D11Device* d3dDevice, int ScrrenWidth, int ScrrenHeight )
{
	bool result;
	//初始化屏幕长宽，纹理长宽，坐标点
	mScrrenWidth = ScrrenWidth;
	mScrrenHeight = ScrrenHeight;

	//初始化顶点缓存，索引缓存
	result = InitializeBuffer( d3dDevice );
	if ( !result )
	{
		MessageBox( NULL, L"Initialize Buffer failure", L"ERROR", MB_OK );
		return false;
	}

	return true;
}

void BitmapClass::Shutdown( )
{
	ShutdownBuffer( );
}

bool BitmapClass::SetDataToIA( ID3D11DeviceContext* d3dDeviceContext )
{
	bool result;

	result = UpdateBuffers( d3dDeviceContext );
	if ( !result )
	{
		return false;
	}
	//设置渲染管线的顶点缓存和索引缓存( IA阶段 )
	SetBuffersToIA( d3dDeviceContext );
	return true;
}

bool BitmapClass::InitializeBuffer( ID3D11Device* d3dDevice )
{
	Vertex* vertexs = NULL;
	WORD* indices = NULL; //一个WORD两个字节 

	mVertexCount = 6;
	mIndexCount = 6;

	//创建顶点数组
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
		return false;

	//创建索引数组
	indices = new WORD[mIndexCount];
	if ( !indices )
		return false;
	
	//初始化顶点数组为0
	memset( vertexs, 0, sizeof( Vertex ) * mVertexCount );

	//赋予索引数组数据
	//注意用左手定则判定是不是背面
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	//第一, 填充( 顶点 )缓存描述和子资源数据结构体, 并创建顶点缓存( 这里用的是动态缓存 )
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof( Vertex ) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

void BitmapClass::ShutdownBuffer( )
{
	//释放顶点缓存和索引缓存
	ReleaseCOM( md3dIndexBuffer );
	ReleaseCOM( md3dVertexBuffer );
}

void BitmapClass::SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext )
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

bool BitmapClass::UpdateBuffers( ID3D11DeviceContext* d3dDeviceContext )
{
	//由于使用延迟渲染, 几何信息存在texture中, 在顶点缓存最原始的数据被改变了( 比如有一部分已经被深度模板剔除了 ), 属于动态顶点缓存, UpdateBuffers函数用于更新顶点缓存
	//求出win32坐标下图片的的left, right, top, bottom坐标, 由WIN32坐标PosX和PosY变为D3D11坐标系
	float left, right, top, bottom;

	left = ( float )( ( mScrrenWidth / 2 ) * -1 );
	right = left + ( float )mScrrenWidth;
	top = ( float )( mScrrenHeight / 2 );
	bottom = top - ( float )mScrrenHeight;

	//创建临时的顶点数组
	Vertex *vertexs;
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
	{
		return false;
	}

	//加载临时顶点数据, 这些是DX11坐标, 即屏幕中心为原点
	vertexs[0].pos = XMFLOAT3( left, top, 0.0f );
	vertexs[0].color = XMFLOAT2( 0.0f, 0.0f );

	vertexs[1].pos = XMFLOAT3( right, bottom, 0.0f );
	vertexs[1].color = XMFLOAT2( 1.0f, 1.0f );

	vertexs[2].pos = XMFLOAT3( left, bottom, 0.0f );
	vertexs[2].color = XMFLOAT2( 0.0f, 1.0f );

	vertexs[3].pos = XMFLOAT3( left, top, 0.0f );
	vertexs[3].color = XMFLOAT2( 0.0f, 0.0f );

	vertexs[4].pos = XMFLOAT3( right, top, 0.0f );
	vertexs[4].color = XMFLOAT2( 1.0f, 0.0f );

	vertexs[5].pos = XMFLOAT3( right, bottom, 0.0f );
	vertexs[5].color = XMFLOAT2( 1.0f, 1.0f );

	//锁定顶点缓存为了可以进行写入（动态缓存不能用UpdateSubResources写入）
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR( d3dDeviceContext->Map( md3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ); // 获取指向子资源md3dVertexBuffer的一个指针mappedResource，并禁止GPU访问该资源

	//获取指向顶点缓存的指针
	Vertex* verticesPtr;
	verticesPtr = ( Vertex* )mappedResource.pData;

	//把数据复制进顶点缓存
	memcpy( verticesPtr, ( void* )vertexs, ( sizeof( Vertex ) * mVertexCount ) ); // 然后把vertexs中的顶点信息拷贝到md3dVertexBuffer

	//解锁顶点缓存
	d3dDeviceContext->Unmap( md3dVertexBuffer, 0 ); // 使之前Map获取到的指针失效，并且使GPU能过继续访问md3dVertexBuffer

	//释放顶点数组
	delete[] vertexs;
	vertexs = NULL;
	return true;
}