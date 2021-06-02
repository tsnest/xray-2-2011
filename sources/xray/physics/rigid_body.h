////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RIGID_BODY_H_INCLUDED
#define RIGID_BODY_H_INCLUDED

#include <xray/physics/collision_shapes.h>

// temporary (by Andy)
#pragma warning (push)
#pragma warning (disable:4251)

class btRigidBody;

namespace xray {
namespace physics {

struct sync_motion_state;

typedef boost::function< void() > body_moved_callback;

class XRAY_PHYSICS_API bt_rigid_body
{
	friend class bullet_physics_world;
public:
					bt_rigid_body					(	bt_collision_shape_ptr shape, 
														btRigidBody* body, 
														sync_motion_state* motion_state,
														float3 const& render_model_offset );
	float4x4		get_transform					( ) const;
	void			set_transform					( float4x4 const& transform );
	void			apply_impulse					( float3 const& impulse, float3 const& point_in_world );
	bool			is_active						( ) const;
	bool			is_static_or_kinematic_object	( ) const;
	void			set_moved_callback				( body_moved_callback callback );
	float3 const&	render_model_offset				( ) const;
private:
	float3						m_render_model_offset;
	btRigidBody*				m_bt_body;
	bt_collision_shape_ptr		m_shape;
	sync_motion_state*			m_motion_state;
}; // class rigid_body

struct XRAY_PHYSICS_API bt_rigid_body_construction_info
{
		float			m_mass;

		bt_collision_shape_ptr	m_collisionShape;
		float				m_linearDamping;
		float				m_angularDamping;

		///best simulation results when friction is non-zero
		float			m_friction;
		///best simulation results using zero restitution.
		float			m_restitution;

		float			m_linearSleepingThreshold;
		float			m_angularSleepingThreshold;

		//Additional damping can help avoiding lowpass jitter motion, help stability for ragdolls etc.
		//Such damping is undesirable, so once the overall simulation quality of the rigid body dynamics system has improved, this should become obsolete
		bool			m_additionalDamping;
		float			m_additionalDampingFactor;
		float			m_additionalLinearDampingThresholdSqr;
		float			m_additionalAngularDampingThresholdSqr;
		float			m_additionalAngularDampingFactor;
		float3			m_render_model_offset;

		bt_rigid_body_construction_info	( );
		bool load						( configs::binary_config_value const& cfg );
#ifndef MASTER_GOLD
		bool load						( configs::lua_config_value const& cfg );
		void save						( configs::lua_config_value& cfg );
#endif
};

XRAY_PHYSICS_API bt_rigid_body* create_rigid_body	( bt_rigid_body_construction_info const& construction_info );
XRAY_PHYSICS_API void			destroy_rigid_body	( bt_rigid_body* body );

} // namespace physics
} // namespace xray
#pragma warning (pop)

#endif // #ifndef RIGID_BODY_H_INCLUDED