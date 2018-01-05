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
	float4 directional_light_vector; // 16
	float4 directional_light_colour; // 16
	float4 ambient_light_colour;	 // 16
	float red_fraction; //4
	float scale; // 4
	float2 packing; //2*4 bytes = 8 bytes
	//total size = 128
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{

	VOut output;
//output.position.xy *= scale;

	color.r *= red_fraction;
	output.position = mul(WVPMatrix, position);

	float diffuse_amount = dot(directional_light_vector, normal); // uses the models normal values // determines light intensity basedn on the angle bewtween the two vectors 
	diffuse_amount = saturate(diffuse_amount);	// ristricts the values to between 0.0f -1.0f
												// so the normals facing away generate a negitive value
	//output.color = color;
	output.color = ambient_light_colour + (directional_light_colour * diffuse_amount); //gets the final colour output for bthe lighting information
	output.texcoord = texcoord;

	return output;

}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) :SV_TARGET
{
	return color * texture0.Sample(sampler0,texcoord); // lighting colour and texture sample colour 
//	return color;									   // returns just the lighting value 
}