Texture2D ColorTexture:register( t0 ); //��ɫ����
Texture2D NormalTexture:register( t1 ); //����������
Texture2D WposTexture:register( t2 ); //����������

SamplerState SampleType:register( s0 ); //������ʽ

#define LIGHT_NUM 100
//VertexShader
cbuffer CBMatrix:register( b0 )
{
	matrix World;
	matrix View;
	matrix Ortho;
};

cbuffer CBDiffuseColor:register( b1 )
{
	float4 diffuseColor[LIGHT_NUM];
};

cbuffer CBLightPos : register( b2 )
{
	float4 LightPos[LIGHT_NUM];
};

struct VertexIn
{
	float3 Pos:POSITION;
	float2 Tex:TEXCOORD0; //���������������������
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

VertexOut VS( VertexIn ina )
{
	VertexOut outa;

	//������任����βü��ռ�
	outa.Pos = mul( float4( ina.Pos, 1.0f ), View );
	outa.Pos = mul( outa.Pos, Ortho );

	//��ȡ��������
	outa.Tex= ina.Tex;
	return outa;
}

float4 PS( VertexOut outa ) : SV_Target
{
	float4 TexColor; //�ɼ���������ɫ
	float4 Normal;
	float4 Wpos;
	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f }; //�����������ɫ

	TexColor = ColorTexture.Sample( SampleType, outa.Tex );
	Normal = NormalTexture.Sample( SampleType, outa.Tex );
	Wpos = WposTexture.Sample( SampleType, outa.Tex );

	float DiffuseFactor[LIGHT_NUM];
	float4 DiffuseColorIntensity[LIGHT_NUM];
	float4 DiffuseColor[LIGHT_NUM];
	float atten1, atten2, atten3; //˥��ϵ��

	//��ʼ��˥��ϵ��
	atten1 = 1.0f;
	atten2 = 1.0f;
	atten3 = 2.0f;

	for( int i = 0; i < LIGHT_NUM; i++ )
	{
		float3 LookDirection = LightPos[i].xyz - Wpos.xyz;
		float distance = length( LookDirection );
		DiffuseColorIntensity[i] = diffuseColor[i] / ( atten1 + atten2 * distance + atten3 * distance * distance ); //˥��ϵ��ע������ż���Ϊ��ĸ
		LookDirection = normalize( LookDirection );
		DiffuseFactor[i] = saturate( dot( Normal, LookDirection ) );
		DiffuseColor[i] = DiffuseFactor[i] * DiffuseColorIntensity[i];
	}

	//����,�õƹ���ɫ����������ɫ
	for(int i = 0; i < LIGHT_NUM; i++)
	{
		color = color + DiffuseColor[i];
	}
	color = saturate( color ) * TexColor * 5;

	return color;
}