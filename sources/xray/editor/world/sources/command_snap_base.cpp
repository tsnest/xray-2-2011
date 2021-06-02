////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_snap_base.h"
#include "object_base.h"
#include "project.h"
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include "collision_object_dynamic.h"
#pragma managed( pop )

namespace xray {
namespace editor {

bool command_snap_base::get_valid_collision	( editor_base::collision_ray_objects^ collision_results, object_base_list^ objects, float& distance )
{
	u32 count = collision_results->count();

	for(u32 i=0; i<count; ++i )
	{
		const xray::collision::ray_object_result& r = collision_results->get(i);
		collision::collision_object const* obj_coll = static_cast_checked<collision::collision_object const*> (r.object);
		ASSERT					( obj_coll, "The obj must not be NULL.");

		if( obj_coll->get_type() & editor_base::collision_object_type_dynamic )
		{
			collision_object_dynamic const* dyn_coll = static_cast_checked<collision_object_dynamic const*>(obj_coll);
			ASSERT( dyn_coll, "The obj must not be NULL.");

			bool not_found = true;
			for each(object_base^ o in objects)
			{
				if( dyn_coll->get_owner_object() == o )
				{	
					not_found = false;
					break;
				}
			}

			if( not_found )
			{
				distance = r.distance;
				return true;
			}
		}
	}

	return false;
}

} // namespace editor
} // namespace xray

