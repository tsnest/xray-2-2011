////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/character_controller.h>
#include "bullet_include.h"
#include "bullet_physics_world.h"
#include <xray/linkage_helper.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/geometry.h>

XRAY_DECLARE_LINKAGE_ID(physics_character_controller)

namespace xray{
namespace physics{

bt_character_controller* create_character_controller( memory::base_allocator& allocator, 
														physics::world* w )
{
	return XRAY_NEW_IMPL(allocator, bt_character_controller)(w);
}

bt_character_controller::bt_character_controller( physics::world* w ):
	m_capsule_height( 1.1f ),
	m_capsule_radius( 0.4f )	// width = 80cm, total height = h+2R = 190cm
{
	m_bt_physics_world	= static_cast_checked<bullet_physics_world*>(w);
}

bt_character_controller::~bt_character_controller( )
{
	xray::collision::delete_geometry_instance	( &allocator(), m_geometry_instance );
}

memory::base_allocator& bt_character_controller::allocator( )
{
	return m_bt_physics_world->allocator();
}

void bt_character_controller::initialize( )
{
	m_ghostObject						= XRAY_NEW_IMPL( allocator(), btPairCachingGhostObject)();
	m_bt_physics_world->get_bt_internal()->getPairCache()->setInternalGhostPairCallback(XRAY_NEW_IMPL( allocator(), btGhostPairCallback)() );
	m_center_of_shape_offset			= float3(0, m_capsule_height/2.0f+m_capsule_radius, 0);
	btConvexShape* capsule				= XRAY_NEW_IMPL( allocator(), btCapsuleShape)(m_capsule_radius, m_capsule_height );
	m_ghostObject->setCollisionShape	( capsule );
	m_ghostObject->setCollisionFlags	( btCollisionObject::CF_CHARACTER_OBJECT );
	m_ghostObject->setFriction			(100);
	btScalar step_height				= 0.35f;
	m_bt_controller						= XRAY_NEW_IMPL( allocator(), btKinematicCharacterController)( m_ghostObject, capsule, step_height );

	m_geometry_instance						= &*xray::collision::new_capsule_geometry_instance( &allocator(), float4x4().identity(), m_capsule_radius, m_capsule_height / 2 );
}


void bt_character_controller::activate( float4x4 const& t )
{
	set_transform				( t );

	m_bt_physics_world->get_bt_internal()->addCollisionObject( m_ghostObject,
											btBroadphaseProxy::CharacterFilter, 
											btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter );

	m_bt_physics_world->get_bt_internal()->addAction( m_bt_controller );
}

void bt_character_controller::deactivate( )
{
	m_bt_physics_world->get_bt_internal()->removeCollisionObject( m_ghostObject );
	m_bt_physics_world->get_bt_internal()->removeAction			( m_bt_controller );
}

float4x4 bt_character_controller::get_transform( )
{
	float4x4 result = from_bullet( m_bt_controller->getGhostObject()->getWorldTransform() );
	result.c.xyz() -= m_center_of_shape_offset;
	m_geometry_instance->set_matrix( result );

	return			result;
}

void bt_character_controller::set_transform	( float4x4 const& transform )
{
	float4x4 m		= transform;
	m.c.xyz()		+= m_center_of_shape_offset;

	m_geometry_instance->set_matrix							( m );
	m_bt_controller->getGhostObject()->setWorldTransform	( from_xray(m) );
}

void bt_character_controller::set_walk_direction( float3 const& direction )
{
	m_bt_controller->setWalkDirection( from_xray(direction) );
}

void bt_character_controller::jump( )
{
	m_bt_controller->jump();
}
collision::geometry_instance& bt_character_controller::get_capsule( )
{
	return *m_geometry_instance;
}

}
}
