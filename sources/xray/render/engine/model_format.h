////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_MODEL_FORMAT_H_INCLUDED
#define XRAY_RENDER_ENGINE_MODEL_FORMAT_H_INCLUDED

namespace xray {
namespace render {

enum platform_id_enum {
	platform_uni,
	platform_windows_dx11,
	platform_xbox360,
	platform_ps3
}; // enum platform_id_enum

enum model_chunk_enum {
	model_chunk_header						= 1,
	model_chunk_vertices					= 3,
	model_chunk_indices						= 4,
	model_chunk_collision_v					= 25,
	model_chunk_collision_i					= 26,
	model_chunk_terrain_data				= 102,
	model_chunk_terrain_data_compressed1	= 103,
	model_anim_bones						= 101,
	model_texture							= 2,
	model_children							= 9,
}; // enum model_chunk_enum

enum mesh_type_enum {
	mt_static_submesh		= 0,
	mt_static_mesh			= 1,
	mt_terrain_cell			= 99,
	mt_user_mesh_editable	= 100,
	mt_user_mesh_wire		= 101,
	mt_skinned_mesh			= 40,
	mt_skinned_submesh_1w	= 44,
	mt_skinned_submesh_2w	= 45,
	mt_skinned_submesh_3w	= 46,
	mt_skinned_submesh_4w	= 47,
}; // enum mesh_type_enum

// quantization helpers
class base_basis
{
public:
	u8						x,y,z;
	void	set				( float3 n )
	{
		n.normalize_safe	( n );
		n					+= 1.0f;
		n					*= 0.5f*255.f;
		s32 nx				= math::floor(n.x);	math::clamp(nx,0,255);	x=u8(nx);
		s32 ny				= math::floor(n.y);	math::clamp(ny,0,255);	y=u8(ny);
		s32 nz				= math::floor(n.z);	math::clamp(nz,0,255);	z=u8(nz);
	}
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

// skinning
#pragma pack( push, 1 )

struct model_header
{
	model_header():bb(math::create_zero_aabb()){}
	u8				platform_id;		
	u8				type;					// mesh type
	u16				version;
	math::aabb		bb;
	math::sphere	bs;
};

struct vert_static
{
	float3	P;
	float3	N;
	float3	T;
	float3	B;
	float2	uv;
};

struct vert_boned_1w			// (3+3+3+3+2)*4 + 2 = 58 bytes
{
	float3	P;
	float3	N;
	float3	T;
	float3	B;
	float2	uv;
	u16		m;
	inline void invalidate()
	{
		P.set(0.0f, 0.0f, 0.0f); 
		N.set(0.0f, 1.0f, 0.0f); 
		T.set(1.0f, 0.0f, 0.0f);
		B.set(1.0f, 0.0f, 1.0f);
		uv.set(0.0f, 0.0f);
		m = 0;
	}
};

struct vert_boned_2w			// 2*2 +(3+3+3+3+1+2)*4 = 64 bytes
{								
	u16		m		[2];
	float3	P;
	float3	N;
	float3	T;
	float3	B;
	float	w;
	float2	uv;
	inline void invalidate()
	{
		P.set(0.0f, 0.0f, 0.0f); 
		N.set(0.0f, 1.0f, 0.0f); 
		T.set(1.0f, 0.0f, 0.0f);
		B.set(1.0f, 0.0f, 1.0f);
		uv.set(0.0f, 0.0f);
		m[0] = 0; m[1] = 0;
		w = 0.0f;
	}
};

struct vert_boned_3w          // 2*3 +(3+3+3+3+1*2+2)*4 = 70 bytes
{
	u16		m		[3];
	float3	P;
	float3	N;
	float3	T;
	float3	B;
	float	w		[2];
	float2	uv;
	inline void invalidate()
	{
		P.set(0.0f, 0.0f, 0.0f); 
		N.set(0.0f, 1.0f, 0.0f); 
		T.set(1.0f, 0.0f, 0.0f);
		B.set(1.0f, 0.0f, 1.0f);
		uv.set(0.0f, 0.0f);
		m[0] = 0; m[1] = 0; m[2] = 0;
		w[0] = 0.0f; w[1] = 0.0f;
	}
};

struct vert_boned_4w       // 2*4 +(3+3+3+3+1*3+2)*4  = 76 bytes
{
	u16		m		[4];
	float3	P;
	float3	N;
	float3	T;
	float3	B;
	float	w		[3];
	float2	uv;
	inline void invalidate()
	{
		P.set(0.0f, 0.0f, 0.0f); 
		N.set(0.0f, 1.0f, 0.0f); 
		T.set(1.0f, 0.0f, 0.0f);
		B.set(1.0f, 0.0f, 1.0f);
		uv.set(0.0f, 0.0f);
		m[0] = 0; m[1] = 0; m[2] = 0; m[3] = 0;
		w[0] = 0.0f; w[1] = 0.0f; w[2] = 0.0f;
	}
};

struct vb_sw_struct {
	float3	P;
	u32		N;
	u32		T;
	u32		B;
	float2  uv;
};

struct hardware_4weights_skinning_vertex {
	float3	P;
	u8		indices[4];
	float	weights[3];
	u32		N;
	u32		T;
	u32		B;
	float2  uv;
}; // struct hardware_4weights_skinning_vertex

#pragma pack(pop)

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_MODEL_FORMAT_H_INCLUDED