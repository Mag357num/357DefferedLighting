Texture2D ShaderTexture:register(t0); //纹理资源
SamplerState SampleType:register(s0); //采样方式

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
	float2 Tex:TEXCOORD0; //多重纹理可以用其它数字
	float3 Normal:NORMAL; // 局部空间法线
};

struct VertexOut
{
	float4 Pos:SV_POSITION;
	float3 Pos_W:POSITION; //点在世界空间的位置
	float2 Tex:TEXCOORD0;
	float3 W_Normal:NORMAL; //世界空间的法线
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

	//将坐标变换到齐次裁剪空间
	outa.Pos = mul(float4(ina.Pos,1.0f), World);
	outa.Pos = mul(outa.Pos, View);
	outa.Pos = mul(outa.Pos, Proj);

	//将法线变换到世界空间
	outa.W_Normal = mul(ina.Normal, (float3x3)WorldInvTranspose);  //此事世界逆转置矩阵的第四行本来就没啥用
	outa.W_Normal = normalize(outa.W_Normal);

	//求点在世界空间的位置
	float4 PosW= mul(float4(ina.Pos, 1.0f), World);
	outa.Pos_W = PosW.xyz;

	outa.Tex= ina.Tex;
	return outa;
}

/*延迟渲染的PixelShader输出的为屏幕上的未经处理的渲染到屏幕的像素和像素对应的法线*/
PixelOut PS(VertexOut outa) : SV_Target
{
	PixelOut pout;

	//第一,获取像素的采样颜色
	pout.color = ShaderTexture.Sample(SampleType, outa.Tex);

	//第二，获取像素的法线量
	pout.normal = float4(outa.W_Normal, 1.0f);

	//第三, 获取世界坐标
	pout.wpos = float4(outa.Pos_W, 1.0f);
	return pout;
}