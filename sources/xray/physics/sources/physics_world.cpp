////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.01.2008
//	Author		: Dmitriy Iassenev
//	Description : world class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "physics_world.h"
#include "shell.h"
#include "element.h"

#include "shell_static.h"
#include "static_element.h"

#include "body_ode.h"
#include "ode_include.h"
#include <ode/memory.h>
#include "scheduler.h"
#include "joint_ball.h"

#include <xray/console_command.h>
#include <xray/configs.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry_instance.h>
#include <xray/collision/space_partitioning_tree.h>

#include "temp_storage_holder.h"
#include "animated_model_instance_cook.h"


//static bool s_debug_render = false;
//static console_commands::cc_bool physicss_debug_render( "physics_debug_render", s_debug_render, true, console_commands::command_type_user_specific );

void* ode_malloc( size_t size )
{
	return XRAY_MALLOC_IMPL( xray::physics::g_ph_allocator, (u32)size, "ode" );
}

void* ode_realoc( void* p, size_t /*oldsize*/, size_t newsize )
{
	return XRAY_REALLOC_IMPL( xray::physics::g_ph_allocator, p, (u32)newsize, "ode" );
}

void	ode_free( void* p, size_t /*size*/ )
{
	return XRAY_FREE_IMPL( xray::physics::g_ph_allocator, p );
}

namespace xray {
namespace physics {

struct shell;

static void register_cooks				( )
{
	static animated_model_instance_cook	animated_model_cook;
	register_cook						( &animated_model_cook );
}

physics_world::physics_world( memory::base_allocator& allocator, engine& engine/*, xray::render::scene_ptr const& scene,	 render::debug::renderer &renderer*/ ) :
	m_engine				( engine ),
	m_allocator				( allocator ),
	//m_space					( 0 ),
	m_space					( collision::new_space_partitioning_tree( &allocator, 2.f, 100 ) ),
	m_temp_storage_holder	( XRAY_NEW_IMPL( allocator, temp_storage_holder)() ),
	m_shell_static			( 0 ),
	m_test_box				( 0 ),
	m_test_sphere			( 0 ),
	m_test_cylinder			( 0 ),
	//m_renderer				( renderer ),
	//m_scene					( scene ),
	m_b_ready				(false)
{
	dSetAllocHandler		( ode_malloc );
	dSetReallocHandler		( ode_realoc );
	dSetFreeHandler			( ode_free );

	m_scheduler				= XRAY_NEW_IMPL( allocator, scheduler )( allocator, *this );
	//m_shell_static = debug_new_static_shell	();
//

//	dWorldSetERP(phWorld, erp(world_spring,world_damping) );
//	dWorldSetCFM(phWorld, CFM(world_spring,world_damping));

	dWorldSetERP( 0, default_erp );
	dWorldSetCFM( 0, default_cfm );
	dWorldSetGravity( 0, 0, -default_gravity, 0 );
	
	register_cooks			( );
}

physics_world::~physics_world( )
{

	XRAY_DELETE_IMPL( m_allocator, m_test_box );
	XRAY_DELETE_IMPL( m_allocator, m_test_sphere );
	XRAY_DELETE_IMPL( m_allocator, m_test_cylinder );

	XRAY_DELETE_IMPL( m_allocator, m_temp_storage_holder );
	if(m_shell_static)
	{
		xray::collision::geometry_instance* temp_gi = &m_shell_static->get_element().get_geometry	( );
		//xray::collision::geometry*			temp_g	= &(*temp_gi->get_geometry());
		//m_collision_world.destroy_geometry( temp_g );
		collision::delete_geometry_instance( &m_allocator, temp_gi );
		XRAY_DELETE_IMPL( m_allocator, m_shell_static );
	}
	
	XRAY_DELETE_IMPL( m_allocator, m_scheduler );
	collision::delete_space_partitioning_tree( &*m_space );
}

void physics_world::set_space( collision::space_partitioning_tree *space )
{
	m_space = space;
}

void physics_world::tick( )
{
	if( !m_b_ready )
		return;
	ASSERT( m_scheduler );
	m_scheduler->tick();

	//if( s_debug_render )
	//	debug_render( scene(), renderer( ) );
}

void physics_world::clear_resources( )
{
}

void physics_world::delete_shell( non_null<xray::physics::shell>::ptr  sh )
{
	xray::physics::shell* temp = &*sh;
	sh->deactivate( *this );
	XRAY_DELETE_IMPL( m_allocator, temp );
}

shell_static* physics_world::debug_new_static_shell()
{
	float4x4 pose = float4x4().identity();
	pose.c.xyz().set(0,-1.f,0) ;

	shell_static* res = XRAY_NEW_IMPL( m_allocator, shell_static )( 
		 *XRAY_NEW_IMPL( m_allocator, static_element )( 
			(*collision::new_sphere_geometry_instance( xray::physics::g_ph_allocator, pose, 1.f ) )
			) 
		);

	res->space_insert( *m_space );
	return res;
}

non_null<physics::shell>::ptr	physics_world::new_static_shell( non_null<collision::geometry_instance>::ptr g_instance )
{
	shell_static* res = XRAY_NEW_IMPL( m_allocator, shell_static )( 
		 *XRAY_NEW_IMPL( m_allocator, static_element )( 
			(*g_instance )
			) 
		);
	
	res->space_insert( *m_space );
	return res;
}

non_null<physics::shell>::ptr physics_world::new_physics_shell( non_null<collision::geometry_instance>::ptr g_instance )
{
		
	body *body	= XRAY_NEW_IMPL( m_allocator, body_ode );
	float4x4 pose	= g_instance->get_matrix();
	body->set_pose( pose );
	non_null<element>::ptr  e = XRAY_NEW_IMPL( m_allocator, element )( *body, *g_instance );
	e->set_mass( 20.f );

	::shell* shell					= XRAY_NEW_IMPL( m_allocator, ::shell )(m_allocator);
	shell->add_element				( e );
	shell->space_insert				( *m_space );
	shell->wake_up					( *m_scheduler );
	return shell;	
}

//non_null<physics::shell>::ptr	physics_world::new_static_shell( non_null<collision::geometry>::ptr geom )
//{
//	float4x4 pose = float4x4().identity();
//	collision::new_geometry_instance( xray::physics::g_allocator, pose, geom  )
//}

non_null<physics::shell>::ptr	physics_world::new_static_shell( const configs::binary_config_value	&cfg )
{
	XRAY_UNREFERENCED_PARAMETER	( cfg );
	return debug_new_static_shell();
}

void	physics_world::debug_render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const 
{
	if( m_space )
		m_space->render( scene, renderer );
}

void	physics_world::create_test_scene( )
{
	fs_new::virtual_path_string							collision_config_path;
	//collision_config_path.assignf						( "resources/animated_model_instances/collision_objects/human_collision.physics");
	//collision_config_path.assignf						( "resources/animated_model_instances/collision_objects/box.physics");

	resources::request requests[] =
	{
		{ "resources/animated_model_instances/collision_objects/box.physics", resources::binary_config_class },
		{ "resources/animated_model_instances/collision_objects/sphere.physics", resources::binary_config_class },
		{ "resources/animated_model_instances/collision_objects/cylinder.physics", resources::binary_config_class },
	};
	
	query_resources	(
		requests,
		array_size( requests ),
		boost::bind( &physics_world::on_create_test_primitives_config_loaded, this, _1 ), 
		&m_allocator
		);

}

void	physics_world::on_create_test_primitives_config_loaded( resources::queries_result& data )
{
	ASSERT( data.is_successful() );
	
	xray::configs::binary_config_ptr box		= static_cast_resource_ptr< configs::binary_config_ptr >( data[0].get_unmanaged_resource() );
	xray::configs::binary_config_ptr sphere		= static_cast_resource_ptr< configs::binary_config_ptr >( data[1].get_unmanaged_resource() );
	xray::configs::binary_config_ptr cylinder	= static_cast_resource_ptr< configs::binary_config_ptr >( data[2].get_unmanaged_resource() );

	m_test_box		= &( *new_physics_shell( box->get_root() ) );
	m_test_sphere	= &( *new_physics_shell( sphere->get_root() ) );
	m_test_cylinder	= &( *new_physics_shell( cylinder->get_root() ) );
	//set_ready( true );
}


void physics_world::deactivate			( ::shell& sh )
{
	
	(sh).remove( *m_scheduler );
}

} // namespace physics
} // namespace xray
