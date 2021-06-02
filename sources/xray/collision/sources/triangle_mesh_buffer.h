////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_BUFFER_H_INCLUDED
#define TRIANGLE_MESH_BUFFER_H_INCLUDED

#include "triangle_mesh_geometry.h"

namespace xray {
namespace collision {

class triangle_mesh_buffer : public triangle_mesh_geometry
{
public:
					triangle_mesh_buffer( memory::base_allocator* allocator,
										  float3 const* vertices,
										  u32 vertex_count,
										  u32 const* indices,
										  u32 index_count,
										  u32 const* triangle_data,
										  u32 triangle_count
					);

private:
	typedef triangle_mesh_geometry		super;
	typedef vectora< math::float3 >		vertices_type;
	typedef vectora< u32 >				indices_type;

private:
	vertices_type	m_vertices;
	indices_type	m_indices;
}; // class world

} // namespace collision
} // namespace xray

#endif // #ifndef TRIANGLE_MESH_BUFFER_H_INCLUDED