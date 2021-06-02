////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OGF_H_INCLUDED
#define XRAY_OGF_H_INCLUDED

namespace xray {

//struct ogf_bbox
//{
//	float3 min;
//	float3 max;
//}; // struct ogf_bbox
//
//struct ogf_bsphere
//{
//	float3 c;
//	float  r;
//}; // struct ogf_bsphere

enum format_version{
	xrOGF_FormatVersion			= 4,
};

struct ogf_header
{
	u8				format_version;			// = xrOGF_FormatVersion
	u8				type;					// MT
	u16				shader_id;				// should not be ZERO
	math::aabb		bb;
	math::sphere	bs;
};

enum enum_ogf_chunk
{
	ogf_chunk_header		= 1,
	ogf_chunk_gcontainer	= 21,
	ogf_children			= 9,
	ogf_children_l			= 10,
	ogf_chunk_vertices		= 3,
	ogf_chunk_indices		= 4,
	ogf_chunk_swi			= 6,
	ogf_texture				= 2,
	ogf_chunk_tree_def		= 12,
	ogf_chunk_collision_v	= 25,
	ogf_chunk_collision_i	= 26,
};

enum enum_mesh_type {
	mt_normal				= 0,
	mt_progressive			= 2,
	mt_hierrarchy			= 1,
	mt_lod					= 6,
	mt_tree_st				= 7,
	mt_skeleton_rigid		= 10,
	mt_terrain_cell			= 99,
	//MT_SKELETON_ANIM		=3,
	//MT_SKELETON_GEOMDEF_PM	=4,
	//MT_SKELETON_GEOMDEF_ST	=5,
	//MT_PARTICLE_EFFECT		=8,
	//MT_PARTICLE_GROUP		=9,
	//MT_TREE_PM				=11,
	//MT_3DFLUIDVOLUME		=12,
};


struct slide_window
{
	u32				offset;
	u16				num_tris;
	u16				num_verts;
};

struct slide_window_item
{
	slide_window*	sw;
	u32				count;
	u32				reserved[4];
	slide_window_item() : sw(0),count(0)	{};
};

// quantization helpers
class base_basis
{
public:
	u8						x,y,z;
	void	set				( float3	n )
	{
		n.normalize_safe	( n );
		n					+= 1.0f;
		n					*= 0.5f*255.f;
		s32 nx				= math::floor(n.x);	math::clamp(nx,0,255);	x=u8(nx);
		s32 ny				= math::floor(n.y);	math::clamp(ny,0,255);	y=u8(ny);
		s32 nz				= math::floor(n.z);	math::clamp(nz,0,255);	z=u8(nz);
	}
	//void	set				(float x, float y, float z)
	//{
	//	float3	n			= { x,y,z };
	//	set					( n );
	//}
	bool	similar			(const base_basis& o);
}; // base_basis

inline std::pair<s16,u8>		s24_tc_base		(float uv)		// [-32 .. +32]
{
	const u32	max_tile	=	32;
	const s32	quant		=	32768/max_tile;

	float		rebased		=	uv*float	(quant);
	s32			_primary	=	math::floor		(rebased);							math::clamp(_primary,		-32768,	32767	);
	s32			_secondary	=	math::floor		(255.5f*(rebased-float(_primary)));	math::clamp(_secondary,	0,		255		);
	return		std::pair<s16,u8>(s16(_primary),u8(_secondary));
}
} // namespace xray

#endif // #ifndef XRAY_OGF_H_INCLUDED