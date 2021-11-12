Texture2D ColorTexture:register(t0); //颜色纹理
Texture2D NormalTexture:register(t1); //法向量纹理

SamplerState SampleType:register(s0); //采样方式

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
	float2 Tex:TEXCOORD0; //多重纹理可以用其它数字
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

VertexOut VS(VertexIn ina)
{
	VertexOut outa;

	//将坐标变换到齐次裁剪空间
	outa.Pos = mul(float4(ina.Pos, 1.0f), View);
	outa.Pos = mul(outa.Pos, Proj);

	//获取纹理坐标
	outa.Tex= ina.Tex;
	return outa;
}

float4 PS(VertexOut outa) : SV_Target
{
	float4 TexColor; //采集的纹理颜色
	float4 Normal;
	float LightFactor; //灯光因子
	float4 color = {0.0f,0.0f,0.0f,0.0f}; //最终输出的颜色

	TexColor = ColorTexture.Sample(SampleType, outa.Tex);
	Normal = NormalTexture.Sample(SampleType, outa.Tex);

	//第三,求出灯光因子
	float3 InvLightDir = -LightDirection;
	LightFactor = saturate(dot(InvLightDir, Normal.xyz));

	//第四,求出灯光照射颜色
	if (LightFactor > 0)
	{
		color += LightFactor*DiffuseColor; //saturate(float1*float4)
	}
	
	color = saturate(color);


	//第五,用灯光颜色调节纹理颜色
	color = color * TexColor;

	return color;
}