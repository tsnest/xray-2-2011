////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_QUAD_H_INCLUDED
#define TERRAIN_QUAD_H_INCLUDED

#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/terrain_collision_utils.h>

namespace xray {
namespace editor {

public delegate void command_finished_delegate_t(u32);
typedef qr<u32, command_finished_delegate_t> command_finished_delegate;


public enum class modifier_shape : int {circle=0, square=1};

ref class terrain_node;

public value struct terrain_node_key
{
				terrain_node_key	( int _x, int _z ):x(_x), z(_z){}
	bool		eq					( terrain_node_key other )		{return x==other.x && z==other.z;}
	int x;
	int z;
}; // struct terrain_node_key

typedef System::Collections::Generic::List<terrain_node_key>			TerrainNodesList;

public value struct terrain_vertex
{
	float		height;

	u8			t0_infl;
	u8			t1_infl;
	u8			t2_infl;

	u8			t0_index;
	u8			t1_index;
	u8			t2_index;

	u32			vertex_color;

	float		tc_shift_x;
	float		tc_shift_z;
	
	u8			get_texture_index		( int layer );
	void		set_texture_index		( int layer, u8 index );
	void		set_default				( );
}; // terrain_vertex


typedef System::Collections::Generic::List<u16>					vert_id_list;

typedef System::Collections::Generic::Dictionary<terrain_node_key, vert_id_list^>		key_vert_id_dict;

typedef System::Collections::Generic::List<terrain_vertex>								vertex_list;

typedef System::Collections::Generic::Dictionary<u16, terrain_vertex>					vertex_dict;
typedef System::Collections::Generic::Dictionary<terrain_node_key, vertex_dict^>		key_vertex_dict;

typedef System::Collections::Generic::Dictionary<u16, float>							height_diff_dict;
typedef System::Collections::Generic::Dictionary<terrain_node_key, height_diff_dict^>	key_height_diff_dict;

public ref struct update_verts_query
{
	u32				id;
	vert_id_list^	list;
};
typedef System::Collections::Generic::List<update_verts_query^>							update_query_list;

struct terrain_quad: public collision::terrain_quad
{
	typedef  collision::terrain_quad super;
	
	bool				setup			( u16 id, terrain_node^ o );
	void				export_lines	( vectora<u16>& dest, u32& start_idx );
	void				export_tris		( vectora<u16>& dest, u32& start_idx, bool clockwise );
	void				export_tris		( vectora<u32>& dest, u32& start_idx, bool clockwise );
	float				min_distance_to_sqr( float3 const& position );
	bool				ray_test		( float3 const& pos, float3 const& dir, float const max_distance, float& range );
	terrain_vertex^		vertex			( u16 index );
	void				debug_draw		( render::debug_vertices_type& vertices, xray::render::debug_indices_type& indices, u32 clr, math::float3 const& camera_pos );

public:
	gcroot<terrain_node^>		owner;
}; // class terrain_quad

public ref struct terrain_import_export_settings
{
	terrain_import_export_settings():m_b_import_mode(false){}
	enum class options{/*rgba_diffuse, rgb_diffuse, rgb_diffuse_a_heightmap, a_heightmap,*/ grayscale_heightmap};
	System::String^		m_source_filename;
	options				m_options;
	bool				m_b_create_cell;

	float				m_min_height;
	float				m_max_height;
	
	int					lt_x;
	int					lt_z;

	int					size_x;
	int					size_z;
	bool				m_b_import_mode;
}; // heightmap_import_settings

inline float distance_xz(float3 const& v0, float3 const& v1)
{	return math::sqrt( math::sqr((v0).x-v1.x) + math::sqr(v0.z-v1.z) ); }

inline float distance_xz_sqr(float3 const& v0, float3 const& v1)
{	return math::sqr((v0).x-v1.x) + math::sqr(v0.z-v1.z); }


enum class get_neighbour_result{ok, missing};

get_neighbour_result get_neighbour(	terrain_node^ terrain,
									u16 const origin_vert_idx, 
									int const& offset_x_idx, 
									int const& offset_z_idx,
									terrain_node^% result_node,
									u16& result_vert_idx );

} //namespace editor
} //namespace xray


#endif // #ifndef TERRAIN_QUAD_H_INCLUDED