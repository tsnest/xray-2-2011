////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_TYPES_H_INCLUDED
#define COMMON_TYPES_H_INCLUDED

namespace xray {
namespace render {

////////////////////////////////////////////////////////////////////////////
// Debug renderer defines
///////////////////////////////////////////////////////////////////////////

struct vertex_colored
{
	math::float3	position;
	u32				color;
}; 

////////////////////////////////////////////////////////////////////////////
// Texture defines
////////////////////////////////////////////////////////////////////////////

static	u32	const max_texture_name_length			= sizeof(string_path) / sizeof(char);
typedef	fixed_string< max_texture_name_length >		texture_string;

////////////////////////////////////////////////////////////////////////////
// Terrain defines
////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct terrain_data
{
	float		height;

	float2		tex_coord0;
	float2		tex_coord1;
	float2		tex_coord2;

	u8			tex_id0;
	u8			tex_id1;
	u8			tex_id2;

	u8			alpha0;
	u8			alpha1;
	u8			alpha2;

	u32			color;

}; // class terrain_data
#pragma pack(pop)

struct buffer_fragment 
{
	u32		start;
	u32		size;
	void*	buffer;

}; // class buffer_fragment

struct buffer_fragment_NEW
{
	buffer_fragment_NEW(xray::memory::base_allocator* allocator):buffer(allocator){}
	vectora<terrain_data>	buffer;
	u32						start;
	u32						size;


}; // class buffer_fragment_NEW

const u32 terrain_texture_max_count		= 16;
const u32 terrain_texture_size			= 1024;

} // namespace render 
} // namespace xray 

#endif // #ifndef COMMON_TYPES_H_INCLUDED