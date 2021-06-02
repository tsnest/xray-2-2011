////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_mesh_buffer.h"

using xray::collision::triangle_mesh_buffer;
using xray::math::float3;
using xray::memory::base_allocator;

triangle_mesh_buffer::triangle_mesh_buffer	(
		base_allocator* allocator,
		float3 const* const vertices,
		u32 const vertex_count,
		u32 const* const indices,
		u32 const index_count,
		u32 const* triangle_data,
		u32 triangle_count
	) :
	super		( allocator ),
	m_vertices	( *allocator, vertices, vertices + vertex_count ),
	m_indices	( *allocator,  indices,  indices  + index_count )
{
	if ( triangle_data )
	{
		R_ASSERT(index_count/3 == triangle_count);
		m_triangle_data.assign(triangle_data, triangle_data+triangle_count);
	}

	initialize	( allocator, &*m_vertices.begin(), m_vertices.size(), &*m_indices.begin(), m_indices.size() );
}