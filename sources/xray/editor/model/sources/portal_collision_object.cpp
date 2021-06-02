////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "portal_collision_object.h"
#include <xray\editor\base\collision_object_types.h>
#include "edit_portal.h"
namespace xray {
namespace model_editor {

	portal_collision_object::portal_collision_object( collision::geometry const* geometry, edit_portal^ portal )
		:super( g_allocator, xray::editor_base::collision_object_type_touch, &*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geometry )),
m_portal( portal )
{

}

portal_collision_object::~portal_collision_object( )
{
	super::destroy				( g_allocator );
}

bool portal_collision_object::touch( ) const
{
	m_portal->set_selected( true );
	return true;
}

} // namespace model_editor
} // namespace xray