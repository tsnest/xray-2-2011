////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/physics/rigid_body.h>
#include "bullet_include.h"

namespace xray {
namespace physics {

struct sync_motion_state :public btMotionState
{
	btTransform				m_WorldTrans;
//	btTransform				m_centerOfMassOffset;
	body_moved_callback		m_callback;

	sync_motion_state( )
	:m_WorldTrans( btTransform::getIdentity() )
//	m_centerOfMassOffset( btTransform::getIdentity() )
	{}

	virtual void getWorldTransform( btTransform& centerOfMassWorldTrans ) const 
	{
		centerOfMassWorldTrans = 	/*m_centerOfMassOffset **/ m_WorldTrans ;
	}

	virtual void setWorldTransform( const btTransform& centerOfMassWorldTrans )
	{
		m_WorldTrans = centerOfMassWorldTrans /** m_centerOfMassOffset.inverse()*/;
		if(m_callback)
			m_callback();
	}
};
	

bt_rigid_body::bt_rigid_body(	bt_collision_shape_ptr shape, 
								btRigidBody* body, 
								sync_motion_state* motion_state,
								float3 const& render_model_offset )
:m_bt_body				( body ),
m_shape					( shape ),
m_motion_state			( motion_state ),
m_render_model_offset	( render_model_offset )
{
	m_bt_body->setUserPointer( this );
}

float3 const& bt_rigid_body::render_model_offset( ) const
{
	return m_render_model_offset;
}

void bt_rigid_body::set_moved_callback( body_moved_callback callback )
{
	m_motion_state->m_callback = callback;
}

bool bt_rigid_body::is_active( ) const
{
	return m_bt_body->isActive();
}

float4x4 bt_rigid_body::get_transform( ) const
{
	float4x4 cmi = math::invert4x3( create_translation(m_render_model_offset) );
	return cmi * from_bullet( m_motion_state->m_WorldTrans );
}

void bt_rigid_body::set_transform( float4x4 const& transform )
{
	m_motion_state->m_WorldTrans		= from_xray( create_translation(m_render_model_offset) * transform );
	m_bt_body->setWorldTransform		( m_motion_state->m_WorldTrans );

	float4x4 m = get_transform( );
}

void bt_rigid_body::apply_impulse( float3 const& impulse, float3 const& point_in_world )
{
	btVector3 rel_pos				= from_xray(point_in_world) - m_bt_body->getCenterOfMassPosition();
	m_bt_body->setActivationState	( ACTIVE_TAG/*DISABLE_DEACTIVATION*/ );
	m_bt_body->applyImpulse			( from_xray(impulse), rel_pos );
}

bool bt_rigid_body::is_static_or_kinematic_object( ) const
{
	return m_bt_body->isStaticOrKinematicObject();
}

void destroy_rigid_body( bt_rigid_body* body )
{
	XRAY_DELETE_IMPL( g_ph_allocator, body );
}

bt_rigid_body* create_rigid_body( bt_rigid_body_construction_info const& construction_info )
{
	sync_motion_state* motion_state		= XRAY_NEW_IMPL( g_ph_allocator, sync_motion_state )();
	btVector3							local_inertia(0,0,0);
	
	bt_collision_shape_ptr shape = construction_info.m_collisionShape;

	shape->get_bt_shape()->calculateLocalInertia( construction_info.m_mass, local_inertia );
	
	btRigidBody::btRigidBodyConstructionInfo	info(	construction_info.m_mass, 
														motion_state, 
														shape->get_bt_shape(),
														local_inertia );

	///When a motionState is provided, the rigid body will initialize its world transform from the motion state
	///In this case, m_startWorldTransform is ignored.

	info.m_localInertia		= local_inertia;
	info.m_linearDamping	= construction_info.m_linearDamping;
	info.m_angularDamping	= construction_info.m_angularDamping;

	///best simulation results when friction is non-zero
	info.m_friction			= construction_info.m_friction;
	///best simulation results using zero restitution.
	info.m_restitution		= construction_info.m_restitution;

	info.m_linearSleepingThreshold		= construction_info.m_linearSleepingThreshold;
	info.m_angularSleepingThreshold		= construction_info.m_angularSleepingThreshold;

	//Additional damping can help avoiding lowpass jitter motion, help stability for ragdolls etc.
	//Such damping is undesirable, so once the overall simulation quality of the rigid body dynamics system has improved, this should become obsolete
	info.m_additionalDamping						= construction_info.m_additionalDamping;
	info.m_additionalDampingFactor				= construction_info.m_additionalDampingFactor;
	info.m_additionalLinearDampingThresholdSqr	= construction_info.m_additionalLinearDampingThresholdSqr;
	info.m_additionalAngularDampingThresholdSqr	= construction_info.m_additionalAngularDampingThresholdSqr;
	info.m_additionalAngularDampingFactor		= construction_info.m_additionalAngularDampingFactor;

	btRigidBody* body							= XRAY_NEW_IMPL(g_ph_allocator, btRigidBody) ( info );
	bt_rigid_body* result						= XRAY_NEW_IMPL(g_ph_allocator, bt_rigid_body)( shape, body, motion_state, construction_info.m_render_model_offset );
	return result;
}

} // namespace physics
} // namespace xray
