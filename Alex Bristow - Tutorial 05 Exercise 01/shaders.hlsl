struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;

};

cbuffer CBuffer0
{
	matrix WVPMatrix;	//64 bytes
	float red_fraction; //4 byte
	float scale;		// 4 byte
	float2 packing; //3 x 4 byte
};

VOut VShader(float4 position : POSITION , float4 color : COLOR)
{
	VOut output;
	color.r *= red_fraction;
	output.position = mul(WVPMatrix, position);
	output.color = color;

	return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}
