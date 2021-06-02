////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BULLET_PHYSICS_WORLD_H_INCLUDED
#define BULLET_PHYSICS_WORLD_H_INCLUDED

#include<xray/physics/world.h>
#include<xray/physics/soft_body.h>

class btCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btDynamicsWorld;
class btSoftRigidDynamicsWorld;
struct btSoftBodyWorldInfo;
class btSoftBody;

namespace xray {
namespace physics {

class bullet_physics_world :public world,
							public boost::noncopyable
{
public:
	
	bullet_physics_world	( memory::base_allocator& allocator, engine& engine );
	virtual	void			initialize				( );
	virtual	void			tick					( );
	virtual	void			clear_resources			( ){};
	virtual	void			debug_render			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const;
	virtual	void			set_ready				( bool ){};
	virtual	void			create_test_scene		( );

	virtual	void			add_rigid_body			( bt_rigid_body* );
	virtual	void			remove_rigid_body		( bt_rigid_body* );
	virtual	void			add_soft_body			( bt_soft_body_rope* );
	virtual	void			remove_soft_body		( bt_soft_body_rope* );

	virtual closest_ray_result	ray_test			( float3 const& ray_from, float3 const& ray_dir, float const ray_length );

	inline memory::base_allocator&		allocator	( )		{ return m_allocator; }
	btSoftRigidDynamicsWorld*	get_bt_internal		( )		{ return m_dynamicsWorld; }
	
	bt_soft_body_rope*			create_soft_body_rope( rope_construction_info const& construction_info );
	void						destroy_soft_body_rope( bt_soft_body_rope* body );

private:
	timing::timer				m_timer;
	memory::base_allocator&		m_allocator;
// bullet physics
	btCollisionConfiguration*	m_collisionConfiguration;
	btCollisionDispatcher*		m_dispatcher;
	btBroadphaseInterface*		m_overlappingPairCache;
	btConstraintSolver*			m_constraintSolver;
	
	btSoftBodyWorldInfo*		m_softBodyWorldInfo;
	btSoftRigidDynamicsWorld*	m_dynamicsWorld;

	btRigidBody*				m_ground_plane_body;
	bt_rigid_body*				m_dynamic_test_body;
	btSoftBody*					m_test_rope_body;

	engine&						m_engine;
	float						m_last_frame_time;
}; // class bullet_physics_world

} // namespace physics
} // namespace xray

#endif // #ifndef BULLET_PHYSICS_WORLD_H_INCLUDED