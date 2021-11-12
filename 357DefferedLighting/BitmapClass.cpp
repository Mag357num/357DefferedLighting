#include"BitmapClass.h"

BitmapClass::BitmapClass( )
{
	md3dVertexBuffer = NULL; //���㻺��
	md3dIndexBuffer = NULL; //��������
	mVertexCount = 0;
	mIndexCount = 0;
}

BitmapClass::~BitmapClass( )
{

}

bool BitmapClass::Initialize( ID3D11Device* d3dDevice, int ScrrenWidth, int ScrrenHeight )
{
	bool result;
	//��ʼ����Ļ���������������
	mScrrenWidth = ScrrenWidth;
	mScrrenHeight = ScrrenHeight;

	//��ʼ�����㻺�棬��������
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
	//������Ⱦ���ߵĶ��㻺�����������( IA�׶� )
	SetBuffersToIA( d3dDeviceContext );
	return true;
}

bool BitmapClass::InitializeBuffer( ID3D11Device* d3dDevice )
{
	Vertex* vertexs = NULL;
	WORD* indices = NULL; //һ��WORD�����ֽ� 

	mVertexCount = 6;
	mIndexCount = 6;

	//������������
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
		return false;

	//������������
	indices = new WORD[mIndexCount];
	if ( !indices )
		return false;
	
	//��ʼ����������Ϊ0
	memset( vertexs, 0, sizeof( Vertex ) * mVertexCount );

	//����������������
	//ע�������ֶ����ж��ǲ��Ǳ���
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	//��һ, ���( ���� )��������������Դ���ݽṹ��, ���������㻺��( �����õ��Ƕ�̬���� )
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

void BitmapClass::ShutdownBuffer( )
{
	//�ͷŶ��㻺�����������
	ReleaseCOM( md3dIndexBuffer );
	ReleaseCOM( md3dVertexBuffer );
}

void BitmapClass::SetBuffersToIA( ID3D11DeviceContext* d3dDeviceContext )
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

bool BitmapClass::UpdateBuffers( ID3D11DeviceContext* d3dDeviceContext )
{
	//����ʹ���ӳ���Ⱦ, ������Ϣ����texture��, �ڶ��㻺����ԭʼ�����ݱ��ı���( ������һ�����Ѿ������ģ���޳��� ), ���ڶ�̬���㻺��, UpdateBuffers�������ڸ��¶��㻺��
	//���win32������ͼƬ�ĵ�left, right, top, bottom����, ��WIN32����PosX��PosY��ΪD3D11����ϵ
	float left, right, top, bottom;

	left = ( float )( ( mScrrenWidth / 2 ) * -1 );
	right = left + ( float )mScrrenWidth;
	top = ( float )( mScrrenHeight / 2 );
	bottom = top - ( float )mScrrenHeight;

	//������ʱ�Ķ�������
	Vertex *vertexs;
	vertexs = new Vertex[mVertexCount];
	if ( !vertexs )
	{
		return false;
	}

	//������ʱ��������, ��Щ��DX11����, ����Ļ����Ϊԭ��
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

	//�������㻺��Ϊ�˿��Խ���д�루��̬���治����UpdateSubResourcesд�룩
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR( d3dDeviceContext->Map( md3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource ) ); // ��ȡָ������Դmd3dVertexBuffer��һ��ָ��mappedResource������ֹGPU���ʸ���Դ

	//��ȡָ�򶥵㻺���ָ��
	Vertex* verticesPtr;
	verticesPtr = ( Vertex* )mappedResource.pData;

	//�����ݸ��ƽ����㻺��
	memcpy( verticesPtr, ( void* )vertexs, ( sizeof( Vertex ) * mVertexCount ) ); // Ȼ���vertexs�еĶ�����Ϣ������md3dVertexBuffer

	//�������㻺��
	d3dDeviceContext->Unmap( md3dVertexBuffer, 0 ); // ʹ֮ǰMap��ȡ����ָ��ʧЧ������ʹGPU�ܹ���������md3dVertexBuffer

	//�ͷŶ�������
	delete[] vertexs;
	vertexs = NULL;
	return true;
}