////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "portal_vertex_collision.h"
#include "edit_portal.h"
#include <xray/editor/base/collision_object_types.h>

namespace xray {
namespace model_editor {

portal_vertex_collision::portal_vertex_collision( collision::geometry const* geometry, edit_portal^ portal, int index )
:super( g_allocator, xray::editor_base::collision_object_type_touch, &*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geometry )),
m_portal( portal ),
m_index( index )
{

}

int portal_vertex_collision::get_index( ) const
{
	return m_index;
}

void portal_vertex_collision::set_index( int index )
{
	m_index = index;
}

bool portal_vertex_collision::touch( ) const
{
	m_portal->on_vertex_touch( m_index );
	return true;
}

} // namespace model_editor
} // namespace xray
