////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_TERRAIN_COLLISION_UTILS_H_INCLUDED
#define XRAY_COLLISION_TERRAIN_COLLISION_UTILS_H_INCLUDED

namespace xray {
namespace collision {

inline bool get_row_col( float cell_size, int dimension,  float3 const& position_local, int& x, int& z );

template< class terrain_data_type >
inline math::aabb	terrain_aabb( terrain_data_type const &terrain_data );

template< class terrain_data_type >
bool terrain_ray_test(  terrain_data_type const &terrain_data, math::float3 const& p_os, math::float3 const& d_ir, float max_distance, float& distance );


struct terrain_quad {

	inline bool	setup	( u16 id, u32 dimension );
	
	u16		quad_id;
	u16		index_lt;
	u16		index_lb;
	u16		index_rt;
	u16		index_rb;

};

inline u16	terrain_vertex_id( int vert_in_row, int const _x_idx, int const _z_idx );
inline void terrain_vertex_xz( int vert_in_row, u16 vertex_id, int& _x_idx, int& _z_idx );
inline void terrain_vertex_xz( int vert_in_row, float cell_size, u16 vertex_id, float& _x, float& _z );

inline u16 quad_index		( int quad_in_row, int x, int y );
inline void get_quad		( terrain_quad &quad, u32 quad_in_row, int x, int y );
inline bool ray_test_quad	( float3 const& v_lt, float3 const& v_rt, float3 const& v_lb, float3 const& v_rb,
							float3 const& pos, float3 const& dir, float const max_distance, float& range );
} // namespace collision
} // namespace xray

#include <xray/collision/terrain_collision_utils_inline.h>

#endif // #ifndef XRAY_COLLISION_TERRAIN_COLLISION_UTILS_H_INCLUDED