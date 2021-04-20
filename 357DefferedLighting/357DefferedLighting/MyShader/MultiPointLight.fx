Texture2D ColorTexture:register( t0 ); //颜色纹理
Texture2D NormalTexture:register( t1 ); //法向量纹理
Texture2D WposTexture:register( t2 ); //法向量纹理

SamplerState SampleType:register( s0 ); //采样方式

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
	float2 Tex:TEXCOORD0; //多重纹理可以用其它数字
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

VertexOut VS( VertexIn ina )
{
	VertexOut outa;

	//将坐标变换到齐次裁剪空间
	outa.Pos = mul( float4( ina.Pos, 1.0f ), View );
	outa.Pos = mul( outa.Pos, Ortho );

	//获取纹理坐标
	outa.Tex= ina.Tex;
	return outa;
}

float4 PS( VertexOut outa ) : SV_Target
{
	float4 TexColor; //采集的纹理颜色
	float4 Normal;
	float4 Wpos;
	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f }; //最终输出的颜色

	TexColor = ColorTexture.Sample( SampleType, outa.Tex );
	Normal = NormalTexture.Sample( SampleType, outa.Tex );
	Wpos = WposTexture.Sample( SampleType, outa.Tex );

	float DiffuseFactor[LIGHT_NUM];
	float4 DiffuseColorIntensity[LIGHT_NUM];
	float4 DiffuseColor[LIGHT_NUM];
	float atten1, atten2, atten3; //衰减系数

	//初始化衰减系数
	atten1 = 1.0f;
	atten2 = 1.0f;
	atten3 = 2.0f;

	for( int i = 0; i < LIGHT_NUM; i++ )
	{
		float3 LookDirection = LightPos[i].xyz - Wpos.xyz;
		float distance = length( LookDirection );
		DiffuseColorIntensity[i] = diffuseColor[i] / ( atten1 + atten2 * distance + atten3 * distance * distance ); //衰减系数注意加括号计算为分母
		LookDirection = normalize( LookDirection );
		DiffuseFactor[i] = saturate( dot( Normal, LookDirection ) );
		DiffuseColor[i] = DiffuseFactor[i] * DiffuseColorIntensity[i];
	}

	//第五,用灯光颜色调节纹理颜色
	for(int i = 0; i < LIGHT_NUM; i++)
	{
		color = color + DiffuseColor[i];
	}
	color = saturate( color ) * TexColor * 5;

	return color;
}