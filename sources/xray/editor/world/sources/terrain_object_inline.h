////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_OBJECT_INLINE_H_INCLUDED
#define TERRAIN_OBJECT_INLINE_H_INCLUDED

#include <xray/collision/terrain_collision_utils.h>

namespace xray {
namespace editor {

inline bool terrain_node::is_in_bound(float3 const& position_local)
{
	float size = (float)m_core->node_size_;
	return ( position_local.x >= 0 && position_local.x <= size &&
			-position_local.z >= 0 && -position_local.z <= size);
}

inline terrain_node_key terrain_core::pick_node(float3 const& point)
{
	terrain_node_key		picked;
	u32 nsz					= node_size_; 
	picked.x				= (int)((point.x) / nsz) -1;
	picked.z				= (int)((point.z) / nsz);
	if(point.z>0.0f)		++picked.z;
	if(point.x>0.0f)		++picked.x;

	return picked;
}

inline float terrain_node::distance_xz_sqr(u16 vertex_id, float3 const& point)
{
	float x,z;
	vertex_xz		(vertex_id, x, z);
	return math::sqr(x-point.x) + math::sqr(z-point.z);
}




inline u16	terrain_node::vertex_id(int const _x_idx, int const _z_idx)
{
	return collision::terrain_vertex_id( m_vertices.m_dimension+1, _x_idx ,_z_idx );
}




inline void terrain_node::vertex_xz(u16 vertex_id, int& _x_idx, int& _z_idx)
{
	collision::terrain_vertex_xz( m_vertices.m_dimension+1, vertex_id, _x_idx, _z_idx );
}



inline void terrain_node::vertex_xz(u16 vertex_id, float& _x, float& _z)
{
	collision::terrain_vertex_xz( m_vertices.m_dimension+1, m_cell_size, vertex_id, _x, _z );
}

inline bool terrain_node::get_row_col( float3 const& position_local, int& x, int& z )
{
//.	ASSERT(	is_in_bound(position_local) );
	return collision::get_row_col( m_cell_size, m_vertices.m_dimension, position_local, x, z );

}

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_OBJECT_INLINE_H_INCLUDED
