////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_object_dynamic.h"
#include "object_composite.h"

namespace xray{
namespace editor{

collision_object_dynamic::collision_object_dynamic(	collision::geometry_instance* geometry,
													object_base^ dynamic,
													collision::object_type t )
:super	( g_allocator, t, geometry ),
m_owner	( dynamic )
{ 	
}

object_base^ collision_object_dynamic::get_owner_object( ) const 
{ 
	object_base^ root_object = m_owner->get_root_object();
	
	if(root_object!=m_owner) // ( in compound? )
	{
		object_composite^ oc = safe_cast<object_composite^>(root_object);
		if(oc->is_edit_mode()) // in edit mode allow to single select and transform modifying
			return m_owner;
		else
			return oc;
	}else
		return m_owner; 
}

bool collision_object_dynamic::touch( ) const
{
	object_base^ root_object = m_owner->get_root_object();
	if(root_object!=m_owner) // ( i am a compound member )
	{
		object_composite^ oc = safe_cast<object_composite^>(root_object);
		return	oc->touch_object(m_owner);
	}
	return false;
}

} //namespace editor
} //namespace xray
