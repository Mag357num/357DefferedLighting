Texture2D ColorTexture:register(t0); //��ɫ����
Texture2D NormalTexture:register(t1); //����������

SamplerState SampleType:register(s0); //������ʽ

//VertexShader
cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
};

cbuffer CBLight:register(b1)
{
	float4 DiffuseColor;
	float3 LightDirection;
	float pad;
}

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

VertexOut VS(VertexIn ina)
{
	VertexOut outa;

	//������任����βü��ռ�
	outa.Pos = mul(float4(ina.Pos, 1.0f), View);
	outa.Pos = mul(outa.Pos, Proj);

	//��ȡ��������
	outa.Tex= ina.Tex;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float4 TexColor; //�ɼ���������ɫ
	float4 Normal;
	float LightFactor; //�ƹ�����
	float4 color = {0.0f,0.0f,0.0f,0.0f}; //�����������ɫ

	TexColor = ColorTexture.Sample(SampleType, outa.Tex);
	Normal = NormalTexture.Sample(SampleType, outa.Tex);

	//����,����ƹ�����
	float3 InvLightDir = -LightDirection;
	LightFactor = saturate(dot(InvLightDir, Normal.xyz));

	//����,����ƹ�������ɫ
	if (LightFactor > 0)
	{
		color += LightFactor*DiffuseColor; //saturate(float1*float4)
	}
	
	color = saturate(color);


	//����,�õƹ���ɫ����������ɫ
	color = color * TexColor;

	return color;
}