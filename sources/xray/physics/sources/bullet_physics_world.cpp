////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bullet_physics_world.h"
#include <xray/physics/rigid_body.h>
#include <xray/physics/soft_body.h>
#include "bullet_include.h"
#include <xray/render/facade/debug_renderer.h>

void* bullet_alloc( size_t size )
{
	return XRAY_MALLOC_IMPL( xray::physics::g_ph_allocator, size, "bullet" );
}

void bullet_free( void* memblock )
{
	return XRAY_FREE_IMPL( xray::physics::g_ph_allocator, memblock );
}

namespace xray {
namespace physics {

btTransform from_xray( float4x4 const& m )
{
	math::quaternion q_xray(m);
	btQuaternion q0		= from_xray(q_xray);
	return btTransform(q0, from_xray(m.c.xyz()) );
}

float4x4 from_bullet( btTransform const& m )
{
	btQuaternion q_bullet	= m.getRotation();
	math::quaternion q_xray = from_bullet( q_bullet );
	return create_rotation(q_xray) * create_translation( from_bullet(m.getOrigin()) );
}


bullet_physics_world::bullet_physics_world( memory::base_allocator& allocator, engine& engine )
:m_engine		( engine ),
m_allocator		( allocator ),
m_dynamic_test_body( NULL ),
m_ground_plane_body	( NULL ),
m_test_rope_body	( NULL )
{
	
}

void bullet_physics_world::initialize( )
{
	btAlignedAllocSetCustom		( bullet_alloc, bullet_free );
	
	btVector3 worldMin			(-1000,-1000,-1000);
	btVector3 worldMax			(1000,1000,1000);

	// discreteDynamiWorld
	//m_collisionConfiguration	= XRAY_NEW_IMPL( m_allocator, btDefaultCollisionConfiguration)();
	//m_dispatcher				= XRAY_NEW_IMPL( m_allocator, btCollisionDispatcher)(m_collisionConfiguration);
	//m_overlappingPairCache		= XRAY_NEW_IMPL( m_allocator, btAxisSweep3)(worldMin,worldMax);
	//m_constraintSolver			= XRAY_NEW_IMPL( m_allocator, btSequentialImpulseConstraintSolver)();
	//m_dynamicsWorld				= XRAY_NEW_IMPL( m_allocator, btDiscreteDynamicsWorld)(m_dispatcher,m_overlappingPairCache,m_constraintSolver,m_collisionConfiguration);


	//SoftDicsreteDynamicWorld
	m_softBodyWorldInfo						= XRAY_NEW_IMPL(m_allocator, btSoftBodyWorldInfo);
	m_softBodyWorldInfo->air_density		=	(btScalar)1.2;
	m_softBodyWorldInfo->water_density		=	0;
	m_softBodyWorldInfo->water_offset		=	0;
	m_softBodyWorldInfo->water_normal		=	btVector3(0,0,0);
	m_softBodyWorldInfo->m_gravity.setValue	(0,-10,0);

	const int maxProxies = 32766;
	m_collisionConfiguration	= XRAY_NEW_IMPL( m_allocator, btSoftBodyRigidBodyCollisionConfiguration)();
	m_dispatcher				= XRAY_NEW_IMPL( m_allocator, btCollisionDispatcher)(m_collisionConfiguration);
	btGImpactCollisionAlgorithm::registerAlgorithm(m_dispatcher);

	m_softBodyWorldInfo->m_dispatcher = m_dispatcher;
	m_overlappingPairCache		= XRAY_NEW_IMPL( m_allocator, btAxisSweep3)(worldMin,worldMax,maxProxies);
	m_softBodyWorldInfo->m_broadphase = m_overlappingPairCache;

	m_constraintSolver			= XRAY_NEW_IMPL( m_allocator, btSequentialImpulseConstraintSolver)();
	m_dynamicsWorld				= XRAY_NEW_IMPL( m_allocator, btSoftRigidDynamicsWorld)(m_dispatcher,
																						m_overlappingPairCache,
																						m_constraintSolver,
																						m_collisionConfiguration );

	m_dynamicsWorld->getDispatchInfo().m_enableSPU = false;//true; //?
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));
	m_softBodyWorldInfo->m_gravity.setValue(0,-10,0);
	m_softBodyWorldInfo->m_sparsesdf.Initialize();
	m_softBodyWorldInfo->m_sparsesdf.Reset();


	//{
	//	btStaticPlaneShape* groundShape = XRAY_NEW_IMPL( m_allocator, btStaticPlaneShape) (btVector3(0,1,0), 0);
	//	btTransform groundTransform;
	//	groundTransform.setIdentity		( );
	//	groundTransform.setOrigin(btVector3(0,5,0));

	//	m_ground_plane_body					= XRAY_NEW_IMPL( m_allocator, btRigidBody) ( 0.0f, NULL, groundShape, btVector3(0,0,0) );
	//	m_ground_plane_body->setWorldTransform( groundTransform );
	//	m_dynamicsWorld->addCollisionObject	( m_ground_plane_body );
	//}

	m_timer.start();
	m_last_frame_time = 0.0f;
}

void bullet_physics_world::tick( )
{
	float const current_time			= m_timer.get_elapsed_sec();
	float const frame_time				= current_time - m_last_frame_time;
	m_last_frame_time					= current_time;
	m_dynamicsWorld->stepSimulation		( frame_time );
	m_softBodyWorldInfo->m_sparsesdf.GarbageCollect();

}

void bullet_physics_world::add_rigid_body( bt_rigid_body* body )
{
	m_dynamicsWorld->addRigidBody( body->m_bt_body );
}

void bullet_physics_world::remove_rigid_body( bt_rigid_body* body )
{
	m_dynamicsWorld->removeRigidBody( body->m_bt_body );
}

void bullet_physics_world::add_soft_body( bt_soft_body_rope* body )
{
	m_dynamicsWorld->addSoftBody( body->m_bt_body );
}

void bullet_physics_world::remove_soft_body( bt_soft_body_rope* body )
{
	m_dynamicsWorld->removeSoftBody( body->m_bt_body );
}

float3 test_body_dim(2,3,5);

void bullet_physics_world::debug_render( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	if(m_test_rope_body)
	{
		for(int i=0;i<m_test_rope_body->m_links.size();++i)
		{
			const btSoftBody::Link&	l=m_test_rope_body->m_links[i];

			float3 const pt_0 = from_bullet(l.m_n[0]->m_x);
			float3 const pt_1 = from_bullet(l.m_n[1]->m_x);
			renderer.draw_line( scene, pt_0, pt_1, math::color(255,0,0,255) );
//			idraw->drawLine(l.m_n[0]->m_x, l.m_n[1]->m_x, lcolor);
		}
	}

	if(!m_dynamic_test_body)
		return;

	float4x4 m = m_dynamic_test_body->get_transform();


	math::color cl_active(255,0,0,255);
	math::color cl_inactive(255,255,255,255);
	renderer.draw_cube(	scene, 
						m, 
						test_body_dim, 
						m_dynamic_test_body->is_active()? cl_active : cl_inactive );

}

void bullet_physics_world::create_test_scene( )
{
#if (1)
	bt_collision_shape_ptr shape	= create_primitive_shape( m_allocator, xray::collision::primitive_box, test_body_dim );

	bt_rigid_body_construction_info	info;
	info.m_collisionShape			= shape;
	info.m_mass						= 50.0f;
	m_dynamic_test_body				= create_rigid_body( info );
	m_dynamic_test_body->set_transform( create_translation(float3(12,15,12)) );
	add_rigid_body					( m_dynamic_test_body );
#endif

#if (1)
// ropes
	float3 pt_0 = float3(-10,10,-20);
	float3 pt_1 = float3(10,12,-20);

	m_test_rope_body =btSoftBodyHelpers::CreateRope(*m_softBodyWorldInfo,	
													from_xray(pt_0),
													from_xray(pt_1),
													16,
													1+2);
	m_test_rope_body->m_cfg.piterations			= 2;
	m_test_rope_body->m_materials[0]->m_kLST	= 0.5f; //0.1+(i/(btScalar)(n-1))*0.9;
	m_test_rope_body->setTotalMass				(20);
	m_dynamicsWorld->addSoftBody				( m_test_rope_body );
#endif

}

closest_ray_result bullet_physics_world::ray_test( float3 const& ray_from, float3 const& ray_dir, float const ray_length )
{
	btVector3 const from	= from_xray( ray_from );
	btVector3 const to		= from_xray( ray_from + ray_dir*ray_length );

	btDiscreteDynamicsWorld::ClosestRayResultCallback cb( from, to);
	m_dynamicsWorld->rayTest( from, to, cb );

	closest_ray_result	result; 
	if(cb.hasHit())
	{
		result.m_hit_point_world	= from_bullet( cb.m_hitPointWorld );
		result.m_object				= static_cast<bt_rigid_body*>(cb.m_collisionObject->getUserPointer());
	}
	return				result;
}

void bullet_physics_world::destroy_soft_body_rope( bt_soft_body_rope* body )
{
	XRAY_DELETE_IMPL( m_allocator, body );
}

bt_soft_body_rope*  bullet_physics_world::create_soft_body_rope( rope_construction_info const& info )
{
	btSoftBody* body = btSoftBodyHelpers::CreateRope( *m_softBodyWorldInfo,	
													from_xray(info.p0),
													from_xray(info.p1),
													info.fragments_count,
													1+2 //??
													);
	body->m_cfg.piterations		= info.iterations;
	body->m_cfg.viterations		= info.iterations;
	body->m_cfg.kVCF			= info.kVCF;
	body->m_cfg.kDP				= info.kDP;	
	body->m_cfg.kDG				= info.kDG;		
	body->m_cfg.kLF				= info.kLF;		
	body->m_cfg.kPR				= info.kPR;		
	body->m_cfg.kVC				= info.kVC;		
	body->m_cfg.kDF				= info.kDF;		
	body->m_cfg.kMT				= info.kMT;		
	body->m_cfg.kCHR			= info.kCHR;	
	body->m_cfg.kKHR			= info.kKHR;
	body->m_cfg.kSHR			= info.kSHR;	
	body->m_cfg.kAHR			= info.kAHR;	
	body->m_cfg.timescale		= info.timescale;

	body->getCollisionShape()->setMargin(info.margin);
	body->m_materials[0]->m_kLST= info.stiftness;

	bt_soft_body_rope* result	= XRAY_NEW_IMPL( m_allocator, bt_soft_body_rope )(body);
	return						result;
}

} // namespace physics
} // namespace xray

