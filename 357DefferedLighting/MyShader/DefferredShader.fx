Texture2D ShaderTexture:register(t0); //������Դ
SamplerState SampleType:register(s0); //������ʽ

//VertexShader
cbuffer CBMatrix:register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
	matrix WorldInvTranspose;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float2 Tex:TEXCOORD0; //���������������������
	float3 Normal:NORMAL; // �ֲ��ռ䷨��
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 Pos_W:POSITION; //��������ռ��λ��
	float2 Tex:TEXCOORD0;
	float3 W_Normal:NORMAL; //����ռ�ķ���
};

struct PixelOut
{
	float4 color:SV_Target0;
	float4 normal:SV_Target1;
	float4 wpos:SV_Target2;
};

VertexOut VS(VertexIn ina)
{
	VertexOut outa;

	//������任����βü��ռ�
	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);

	//�����߱任������ռ�
	outa.W_Normal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //����������ת�þ���ĵ����б�����ûɶ��
	outa.W_Normal = normalize(outa.W_Normal);

	//���������ռ��λ��
	float4 PosW= mul(float4(ina.Pos, 1.0f), World);
	outa.Pos_W = PosW.xyz;

	outa.Tex= ina.Tex;
	return outa;
}

/*�ӳ���Ⱦ��PixelShader�����Ϊ��Ļ�ϵ�δ���������Ⱦ����Ļ�����غ����ض�Ӧ�ķ���*/
PixelOut PS(VertexOut outa) : SV_Target
{
	PixelOut pout;

	//��һ,��ȡ���صĲ�����ɫ
	pout.color = ShaderTexture.Sample(SampleType, outa.Tex);

	//�ڶ�����ȡ���صķ�����
	pout.normal = float4(outa.W_Normal, 1.0f);

	//����, ��ȡ��������
	pout.wpos = float4(outa.Pos_W, 1.0f);
	return pout;
}