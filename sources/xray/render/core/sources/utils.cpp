////////////////////////////////////////////////////////////////////////////
//	Created		: 08.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/utils.h>

//////////////////////////////////////////////////////////////////////////
namespace xray {
namespace render {
namespace utils {

u32 s_format_4x4_pixel[100] = 
{
//	ms-help://MS.VSCC.v90/MS.VSIPCC.v90/MS.Windows_Graphics.February.2010.1033/Windows_Graphics/DXGI_FORMAT.htm
// 	DXGI_FORMAT_UNKNOWN = 0,
	0,
// 	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	16*16,
// 	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	16*16,
// 	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	16*16,
// 	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	16*16,
// 	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	12*16,
// 	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	12*16,
// 	DXGI_FORMAT_R32G32B32_UINT = 7,
	12*16,
// 	DXGI_FORMAT_R32G32B32_SINT = 8,
	12*16,
// 	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	8*16,
// 	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	8*16,
// 	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	8*16,
// 	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	8*16,
// 	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	8*16,
// 	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	8*16,
// 	DXGI_FORMAT_R32G32_TYPELESS = 15,
	8*16,
// 	DXGI_FORMAT_R32G32_FLOAT = 16,
	8*16,
// 	DXGI_FORMAT_R32G32_UINT = 17,
	8*16,
// 	DXGI_FORMAT_R32G32_SINT = 18,
	8*16,
// 	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	8*16,
// 	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	8*16,
// 	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	8*16,
// 	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	8*16,
// 	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	4*16,
// 	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	4*16,
// 	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	4*16,
// 	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	4*16,
// 	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	4*16,
// 	DXGI_FORMAT_R16G16_TYPELESS = 33,
	4*16,
// 	DXGI_FORMAT_R16G16_FLOAT = 34,
	4*16,
// 	DXGI_FORMAT_R16G16_UNORM = 35,
	4*16,
// 	DXGI_FORMAT_R16G16_UINT = 36,
	4*16,
// 	DXGI_FORMAT_R16G16_SNORM = 37,
	4*16,
// 	DXGI_FORMAT_R16G16_SINT = 38,
	4*16,
// 	DXGI_FORMAT_R32_TYPELESS = 39,
	4*16,
// 	DXGI_FORMAT_D32_FLOAT = 40,
	4*16,
// 	DXGI_FORMAT_R32_FLOAT = 41,
	4*16,
// 	DXGI_FORMAT_R32_UINT = 42,
	4*16,
// 	DXGI_FORMAT_R32_SINT = 43,
	4*16,
// 	DXGI_FORMAT_R24G8_TYPELESS = 44,
	4*16,
// 	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	4*16,
// 	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	4*16,
// 	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	4*16,
// 	DXGI_FORMAT_R8G8_TYPELESS = 48,
	2*16,
// 	DXGI_FORMAT_R8G8_UNORM = 49,
	2*16,
// 	DXGI_FORMAT_R8G8_UINT = 50,
	2*16,
// 	DXGI_FORMAT_R8G8_SNORM = 51,
	2*16,
// 	DXGI_FORMAT_R8G8_SINT = 52,
	2*16,
// 	DXGI_FORMAT_R16_TYPELESS = 53,
	2*16,
// 	DXGI_FORMAT_R16_FLOAT = 54,
	2*16,
// 	DXGI_FORMAT_D16_UNORM = 55,
	2*16,
// 	DXGI_FORMAT_R16_UNORM = 56,
	2*16,
// 	DXGI_FORMAT_R16_UINT = 57,
	2*16,
// 	DXGI_FORMAT_R16_SNORM = 58,
	2*16,
// 	DXGI_FORMAT_R16_SINT = 59,
	1*16,
// 	DXGI_FORMAT_R8_TYPELESS = 60,
	1*16,
// 	DXGI_FORMAT_R8_UNORM = 61,
	1*16,
// 	DXGI_FORMAT_R8_UINT = 62,
	1*16,
// 	DXGI_FORMAT_R8_SNORM = 63,
	1*16,
// 	DXGI_FORMAT_R8_SINT = 64,
	1*16,
// 	DXGI_FORMAT_A8_UNORM = 65,
	1*16,
// 	DXGI_FORMAT_R1_UNORM = 66,
	1*2,
// 	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	4*16,
// 	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	4*16,
// 	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	4*16,
//	ms-help://MS.VSCC.v90/MS.VSIPCC.v90/MS.Windows_Graphics.February.2010.1033/Windows_Graphics/d3d10_graphics_programming_guide_resources_block_compression.htm
// 	DXGI_FORMAT_BC1_TYPELESS = 70,
	8,
// 	DXGI_FORMAT_BC1_UNORM = 71,
	8,
// 	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	8,
// 	DXGI_FORMAT_BC2_TYPELESS = 73,
	16,
// 	DXGI_FORMAT_BC2_UNORM = 74,
	16,
// 	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	16,
// 	DXGI_FORMAT_BC3_TYPELESS = 76,
	16,
// 	DXGI_FORMAT_BC3_UNORM = 77,
	16,
// 	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	16,
// 	DXGI_FORMAT_BC4_TYPELESS = 79,
	8,
// 	DXGI_FORMAT_BC4_UNORM = 80,
	8,
// 	DXGI_FORMAT_BC4_SNORM = 81,
	8,
// 	DXGI_FORMAT_BC5_TYPELESS = 82,
	16,
// 	DXGI_FORMAT_BC5_UNORM = 83,
	16,
// 	DXGI_FORMAT_BC5_SNORM = 84,
	16,
// 	DXGI_FORMAT_B5G6R5_UNORM = 85,
	2*16,
// 	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	2*16,
// 	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	4*16,
// 	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	4*16,
// 	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	4*16,
// 	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	4*16,
// 	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	4*16,
// 	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	4*16,
// 	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	4*16,
// 	DXGI_FORMAT_BC6H_TYPELESS = 94,
	0,
// 	DXGI_FORMAT_BC6H_UF16 = 95,
	0,
// 	DXGI_FORMAT_BC6H_SF16 = 96,
	0,
// 	DXGI_FORMAT_BC7_TYPELESS = 97,
	0,
// 	DXGI_FORMAT_BC7_UNORM = 98,
	0,
// 	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	0,
};

u32 get_format_4x4_pixel_size( DXGI_FORMAT format)
{
	return s_format_4x4_pixel[(u32)format];
}

inline u32 calc_texture_size_uncompressed	( u32 width, u32 height, DXGI_FORMAT format, u32 mips, u32 array_count)
{
	float mip_factor = ( 1.f/( math::pow( 4.f,(float)mips))-1.f) / ( 1.f/4.f - 1.f);
	return  u32(mip_factor*width*height*get_format_4x4_pixel_size( format)/16*array_count); 
}

inline u32 is_block_texture_compressed		( DXGI_FORMAT format)
{
	return format >= DXGI_FORMAT_BC1_TYPELESS && format <= DXGI_FORMAT_BC5_SNORM;
}

u32 calc_texture_size_compressed ( u32 width, u32 height, DXGI_FORMAT format, u32 mips, u32 row_min_pitch, u32 surf_min_pitch)
{
	u32 width_ceil	= math::max( width/4, 1u);
	u32 height_ceil = math::max( height/4, 1u);

	u32 row_pitch	= width_ceil*get_format_4x4_pixel_size(format);
		row_pitch	= math::max( row_pitch, row_min_pitch);
	
	u32 top_mip_size = height_ceil*row_pitch;

	top_mip_size	= math::max( top_mip_size, surf_min_pitch);

	if( mips > 1)
		top_mip_size += calc_texture_size_compressed ( width/2, height/2, format, mips-1, row_min_pitch, surf_min_pitch);

	return top_mip_size;
}

u32 calc_texture_size ( u32 width, u32 height, DXGI_FORMAT format, u32 mips, u32 array_count, u32 row_min_pitch, u32 surf_min_pitch)
{
	if( is_block_texture_compressed( format) )
		return array_count*calc_texture_size_compressed( width, height, format, mips, row_min_pitch, surf_min_pitch);
	else
		return  calc_texture_size_uncompressed( width, height, format, mips, array_count); 
}

u32 calc_surface_size ( u32 width, u32 height, DXGI_FORMAT format, u32 row_min_pitch, u32 & row_pitch)
{
	if( format >= DXGI_FORMAT_BC1_TYPELESS && format <= DXGI_FORMAT_BC5_SNORM )
	{
		u32 width_ceil	= math::max( width/4, 1u);
		u32 height_ceil = math::max( height/4, 1u);

		row_pitch	= width_ceil*get_format_4x4_pixel_size(format);
		row_pitch	= math::max( row_pitch, row_min_pitch);

		return  height_ceil*row_pitch;
	}
	else
		return u32(width*height*get_format_4x4_pixel_size( format)/16); 
}

math::float4x4 get_projection_invert( math::float4x4 const & projection)
{
	float4x4 result;
	result.identity			( );
	
	R_ASSERT				( projection.elements[0][0] != 0 );
	result.elements[0][0]	= 1.f/projection.elements[0][0];

	R_ASSERT				( projection.elements[1][1] != 0 );
	result.elements[1][1]	= 1.f/projection.elements[1][1];

	result.elements[2][2]	= 0.f;

	R_ASSERT_CMP			( projection.elements[3][2], !=, 0.0f );
	float const cached1		= 1.f/projection.elements[3][2];

	R_ASSERT_CMP			( projection.elements[2][3], !=, 0.0f );
	float const cached2		= 1.f/projection.elements[2][3];

	result.elements[2][3]	= cached1;
	result.elements[3][2]	= cached2;
	result.elements[3][3]	= -projection.elements[2][2]*cached1*cached2;

	return					result;
}

} // namespace utils
} // namespace render 
} // namespace xray 