////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PHYSICS_CHARACTER_CONTROLLER_H_INCLUDED
#define XRAY_PHYSICS_CHARACTER_CONTROLLER_H_INCLUDED

#include <xray/physics/api.h>

class btKinematicCharacterController;
class btPairCachingGhostObject;

namespace xray {
namespace collision{ class geometry_instance; class geometry; }
namespace physics {

class bullet_physics_world;

class XRAY_PHYSICS_API bt_character_controller
{
public:
				bt_character_controller	( physics::world* w );
				~bt_character_controller( );
	void initialize						( );
	void activate						( float4x4 const& transform );
	void deactivate						( );
	memory::base_allocator&				allocator();
	float4x4							get_transform	( );
	void								set_transform	( float4x4 const& transform );
	void								set_walk_direction	( float3 const& direction );
	void								jump			( );
	collision::geometry_instance &		get_capsule		( );
private:
	btPairCachingGhostObject*			m_ghostObject;
	btKinematicCharacterController*		m_bt_controller;
	float3								m_center_of_shape_offset;
	bullet_physics_world*				m_bt_physics_world;
	float								m_capsule_height;
	float								m_capsule_radius;
	collision::geometry_instance*		m_geometry_instance;
}; // class bt_character_controller

XRAY_PHYSICS_API bt_character_controller* create_character_controller( memory::base_allocator& allocator, physics::world* w );

} // namespace physics
} // namespace xray

#endif // #ifndef XRAY_PHYSICS_CHARACTER_CONTROLLER_H_INCLUDED