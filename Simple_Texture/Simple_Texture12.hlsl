Texture2D<float4> shaderResource;
SamplerState sampl;

//cbuffer cdata
//{
//	float4 data;
//};

struct VSInput
{
	float4 position : Position;
	float2 texcoord : Texcoord;
};

struct VSOutput
{
	float4 position : SV_Position;
	float2 texcoord : Texcoord;
};

VSOutput VSMain(VSInput input)
{
	VSOutput output;
	output.position = input.position;
	output.texcoord = input.texcoord;
	return output;
}

struct PSInput
{
	float4 position : SV_Position;
	float2 texcoord : Texcoord;
};

struct PSOutput
{
	float4 color : SV_Target;	
};

PSOutput PSMain(PSInput input)
{
	PSOutput output;
	output.color = shaderResource.Sample(sampl, input.texcoord);
	//output.color = data;//float4(1.0, 0.4, 0.2, 1.0);
	return output;
}
