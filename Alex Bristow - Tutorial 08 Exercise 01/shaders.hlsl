//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
///Global Varibles
////////////////////////////////////////////////////////////////////////////////

Texture2D texture0;
SamplerState sampler0;


struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord: TEXCOORD;
};
cbuffer CBuffer0
{
	matrix WVPMatrix; // 64 bytes (4*4 = 16 * 4 bytes)
	float red_fraction;
	float scale;
	// 4bytes
	float2 packing; //2*4 bytes = 8 bytes

};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{

	VOut output;


	color.r *= red_fraction;
	output.position = mul(WVPMatrix, position);
	//output.position.xy *= scale;
	output.color = color;
	output.texcoord = texcoord;

	return output;

}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) :SV_TARGET
{
	return color * texture0.Sample(sampler0,texcoord);
}