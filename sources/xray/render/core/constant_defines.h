////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTANT_DEFINES_H_INCLUDED
#define CONSTANT_DEFINES_H_INCLUDED

#include <xray/render/core/shader_defines.h>
#include <xray/render/core/sources/float4x4b.h>

namespace xray {
namespace render_dx10 {

enum	enum_constant_type
{
	rc_float		= 0,
	rc_int			= 1,
	rc_bool			= 2,
// 	rc_sampler		= 99,	//	DX9 shares index for sampler and texture
// 	rc_dx10texture	= 100	//	For DX10 sampler and texture are different resources
	rc_INVALID		= 0xFFFF,
};

// lower	6 bits determine size of the specified enum class.
// high		2 bits used to difference classes ids with the same size and to put them into 
// groups in which object can be casted down ( for example float4 -> float2).
// this hack is used to be able store the class into u8.
// use constant_class_size_mask to retrieve the size of a class variable.
const u32 constant_class_size_shift = 0x08;
const u32 constant_class_size_mask	= 0x00FF;
const u32 constant_class_cast_mask	= 0xFF00;

enum	enum_constant_class //??? maybe enum_constant_struct
{ 
	rc_1x1		= (0 << constant_class_size_shift)	+ (1*1*sizeof(float)),		// vector1, or scalar
	rc_1x2		= (0 << constant_class_size_shift)	+ (1*2*sizeof(float)),		// vector2
	rc_1x3		= (0 << constant_class_size_shift)	+ (1*3*sizeof(float)),		// vector3
	rc_1x4		= (0 << constant_class_size_shift)	+ (1*4*sizeof(float)),		// vector4
													 
	rc_2x4		= (1 << constant_class_size_shift)	+ (2*4*sizeof(float)),		// 4x2 matrix, transpose
	rc_3x4		= (1 << constant_class_size_shift)	+ (3*4*sizeof(float)),		// 4x3 matrix, transpose
	rc_4x4		= (1 << constant_class_size_shift)	+ (4*4*sizeof(float)),		// 4x4 matrix, transpose
												 
 	rc_1x4a		= (2 << constant_class_size_shift)	+ (1*4*sizeof(float)),		// array: vector4
 	rc_3x4a		= (2 << constant_class_size_shift)	+ (3*4*sizeof(float)),		// array: 4x3 matrix, transpose
 	rc_4x4a		= (2 << constant_class_size_shift)	+ (4*4*sizeof(float))		// array: 4x4 matrix, transpose
};				  


//{
// 	//	Don't change this since some code relies on magic numbers
// 	rc_dest_pixel					= ( 1<<0),
// 	rc_dest_vertex					= ( 1<<1),
// 	rc_dest_sampler					= ( 1<<2),	//	For DX10 it's either sampler or texture
// 	rc_dest_geometry				= ( 1<<3),	//	DX10 only
// 	rc_dest_pixel_cb_index_mask		= 0xF000,	//	Buffer index == 0..14
// 	rc_dest_pixel_cb_index_shift	= 12,
// 	rc_dest_vertex_cb_index_mask	= 0x0F00,	//	Buffer index == 0..14
// 	rc_dest_vertex_cb_index_shift	= 8,
// 	rc_dest_geometry_cb_index_mask	= 0x00F0,	//	Buffer index == 0..14
// 	rc_dest_geometry_cb_index_shift	= 4,
//};

enum	//	Constant buffer index masks
{
	cb_buffer_index_mask		= 0xF,	//	Buffer index == 0..14
	cb_buffer_type_mask			= 0x30,
	cb_buffer_pixel_shader		= 0x10,
	cb_buffer_vertex_shader		= 0x20,
	cb_buffer_geometry_shader	= 0x30
};

enum	//	Constant buffers max count
{
	cb_buffer_max_count = 15
};


//////////////////////////////////////////////////////////////////////////										
//  constant_type_traits
																												// use this size instead of sizeof(T)
template < typename T >	 struct constant_type_traits { enum { value = false, type = (u32)-1,	cls = (u32)-1,	size = (u32)-1; }; };

template <> struct constant_type_traits<float>			{ enum { value = true, 	type = rc_float, 	cls = rc_1x1,	size = sizeof(float)		}; };
template <> struct constant_type_traits<math::float2>	{ enum { value = true, 	type = rc_float, 	cls = rc_1x2,	size = sizeof(float2)		}; };
template <> struct constant_type_traits<math::float3>	{ enum { value = true, 	type = rc_float, 	cls = rc_1x3,	size = sizeof(float3)		}; };
template <> struct constant_type_traits<math::float4>	{ enum { value = true, 	type = rc_float, 	cls = rc_1x4,	size = sizeof(float4)		}; };
template <> struct constant_type_traits<float4x4b>		{ enum { value = true, 	type = rc_float, 	cls = rc_4x4,	size = sizeof(float4x4)		}; }; // Not float4x4b
template <> struct constant_type_traits<int>			{ enum { value = true, 	type = rc_int,		cls = rc_1x1,	size = sizeof(int)			}; };
template <> struct constant_type_traits<math::int2>		{ enum { value = true, 	type = rc_int,		cls = rc_1x2,	size = sizeof(math::int2)	}; };
template <> struct constant_type_traits<u32>			{ enum { value = true, 	type = rc_int,		cls = rc_1x1,	size = sizeof(u32)			}; };
template <> struct constant_type_traits<math::uint2>	{ enum { value = true, 	type = rc_int,		cls = rc_1x2,	size = sizeof(math::uint2)	}; };

	
} // namespace render
} // namespace xray

#endif // #ifndef CONSTANT_DEFINES_H_INCLUDED