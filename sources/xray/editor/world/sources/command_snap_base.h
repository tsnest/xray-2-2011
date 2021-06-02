////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_SNAP_BASE_H_INCLUDED
#define COMMAND_SNAP_BASE_H_INCLUDED

#include "project_defines.h"
#include "object_base.h"

namespace xray {
namespace editor {

class collision_object_geometry;

public ref class command_snap_base abstract:	public xray::editor_base::command
{
	virtual	~command_snap_base		( ){}
protected:
		bool get_valid_collision	(	editor_base::collision_ray_objects^ collision_results, 
										object_base_list^ objects, 
										float& distance );

}; // class command_snap_base

ref class object_height_predicate:public System::Collections::Generic::IComparer<object_base^>
{
public:
	virtual int Compare( object_base^ first, object_base^ second )
	{
		float h1 = (first)->get_transform().c.xyz().y;
		float h2 = (second)->get_transform().c.xyz().y;
		if(h1==h2) return 0;
		else
			return (h1>h2)?1:-1;
	}
};

} // namespace editor
} // namespace xray

#endif // #ifndef COMMAND_SNAP_BASE_H_INCLUDED