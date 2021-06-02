////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_STATIC_H_INCLUDED
#define COLLISION_STATIC_H_INCLUDED

#include <xray/physics/shell.h>

namespace xray {

namespace collision {
	class object;
	class geometry;
	class geometry_instance;
} // namespace collision

namespace physics {
	struct world;
} // namespace physics
} // namespace xray

namespace stalker2 {

class collision_static {

public:
			collision_static	( );
	void	create				( resources::managed_resource_ptr const mesh_resource, physics::world &w  );
	void	destroy				( physics::world &w );
private:
	collision::geometry*				m_collision;
	collision::geometry_instance*		m_instance;
	collision::object*					m_collision_object;
	physics::shell*						m_statics_shell;
}; // class collision_static

class object_collision
{
};

} // namespace stalker2

#endif // #ifndef COLLISION_STATIC_H_INCLUDED