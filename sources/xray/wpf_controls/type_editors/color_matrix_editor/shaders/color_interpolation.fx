/// <class>color_interpolation_effect</class>

/// <summary>Color1</summary>
/// <type>Color</type>
/// <defaultValue>#FFFF0000</defaultValue>
float4 color1 : register(C0);
/// <summary>Color1</summary>
/// <type>Color</type>
/// <defaultValue>#FFFF0000</defaultValue>
float4 color2 : register(C1);
/// <summary>Color1</summary>
/// <type>Color</type>
/// <defaultValue>#FF0000FF</defaultValue>
float4 color3 : register(C2);
/// <summary>Color1</summary>
/// <type>Color</type>
/// <defaultValue>#FF0000FF</defaultValue>
float4 color4 : register(C3);

float4 main(float2 uv : TEXCOORD) : COLOR 
{ 
	float4 	top_color 		= lerp( color1, color3, uv.x );
	float4 	bottom_color 	= lerp( color2, color4, uv.x );
	float4  uv_color		= lerp( top_color, bottom_color, uv.y );
	
	return uv_color;
}