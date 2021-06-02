////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.02.2008
//	Author		: Konstantin Slipchenko
//	Description : shell
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "shell.h"

#include "joint.h"
#include "shell_element.h"
#include "scheduler.h"
#include "island.h"
#include <xray/collision/geometry_instance.h>
#include "physics_world.h"
#include "body.h"

shell::shell( memory::base_allocator& allocator )
:m_allocator		( allocator ),
m_update_object		( 0 ),
m_elements			( allocator ),
m_joints			( allocator )
{
	
}

shell::~shell( )
{
	{
	joint_vec::iterator		i = m_joints.begin();
	joint_vec::const_iterator	e = m_joints.end();
	for( ;i!= e; ++i )
		XRAY_DELETE_IMPL			( m_allocator, *i );
	}

	{
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();
	for( ;i!= e; ++i )
		XRAY_DELETE_IMPL			( m_allocator, *i );
	}
}

void shell::move( xray::math::float3 const &delta )
{
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();
	for( ;i!= e; ++i )
	{
		body *b = (*i)->get_body();
		ASSERT( b );
		b->move_position( delta );
	}
}

void	shell::island_step_start	( island	&island, float time_delta )
{
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();;
	for( ;i!= e; ++i )
		(*i)->on_step_start( island, time_delta );
}

void	shell::island_step_end( island &i, float time_delta )
{
	dynamic_object::island_step_end( i, time_delta );
	force_integrate( time_delta );
	position_integrate( i, time_delta );
}

void	shell::force_integrate( float time_delta )
{
	joint_vec::iterator		i = m_joints.begin();
	joint_vec::const_iterator	e = m_joints.end();
	for( ;i!= e; ++i )
		(*i)->integrate( time_delta );
}

void	shell::position_integrate( island &isl, float time_delta )
{
	
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();
	bool b_wake_up = false;
	for( ;i!= e; ++i )
		b_wake_up = (*i)->integrate( isl.step_counter(), time_delta )||b_wake_up;

	if( !awake() && b_wake_up )
		wake_up( isl.get_scheduler() );

	if(!b_wake_up)
		sleep_internal();

	
}

bool	shell::collide( island& isl,  collide_element_interface& ce, bool reverted_order )
{
	bool result = false;	
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();
	for( ;i!= e; ++i )
		result = ce.collide( isl, *(*i), !reverted_order ) || result;
	return result;

}

bool shell::collide_detail( island& isl,  space_object& o, bool reverted_order )
{
	bool result = false;
	element_vec::iterator		i = m_elements.begin();
	element_vec::const_iterator	e = m_elements.end();
	for( ;i!= e; ++i )
		result = o.collide( isl, *(*i), !reverted_order ) || result;
	return result;

}

void shell::on_island_connect		( island	&isl )
{
	{
		joint_vec::iterator		i = m_joints.begin();
		joint_vec::const_iterator	e = m_joints.end();
		for( ;i!= e; ++i )
			(*i)->fill_island( isl );
	}
	{
		element_vec::iterator		i = m_elements.begin();
		element_vec::const_iterator	e = m_elements.end();
		for( ;i!= e; ++i )
			(*i)->fill_island( isl );
	}
}


void	shell::render( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer )const
{
	{
		element_vec::const_iterator		i = m_elements.begin();
		element_vec::const_iterator	e = m_elements.end();
		for( ;i!= e; ++i )
			(*i)->render( scene, renderer );
	}
	{
		joint_vec::const_iterator	i = m_joints.begin();
		joint_vec::const_iterator	e = m_joints.end();
		for( ;i!= e; ++i )
			(*i)->render( scene, renderer );
	}
}

void	shell::wake_up( scheduler &s )
{
	dynamic_object::wake_up( s );
}

math::aabb&	shell::aabb				( math::aabb& box )const
{
	
	box.invalidate();
	element_vec::const_iterator		i = m_elements.begin();
	element_vec::const_iterator		e = m_elements.end();
	for( ;i!= e; ++i )
	{
		math::aabb tmp(math::create_zero_aabb());
		box.modify(
			(*i)->aabb( tmp ) 
		);
	}
	
	return box;
}

void	shell::aabb				( float3 &center, float3 &radius )const
{
	math::aabb box(math::create_invalid_aabb());
	aabb( box );
	center = box.center( radius );
}

bool	shell::aabb_test( math::aabb const& _aabb ) const
{
	math::aabb box(math::create_invalid_aabb());
	if( !aabb( box ).intersect( _aabb ) )
		return false;
	return true;
	/*
	element_vec::const_iterator		i = m_elements.begin();
	element_vec::const_iterator		e = m_elements.end();
	for( ;i!= e; ++i )
		if( (*i)->aabb_test( _aabb ) ) 
			return true;
	return false;
	*/
}

void	shell::space_insert	( xray::collision::space_partitioning_tree &space )
{ 
	dynamic_object::space_insert( space ); 
}

void shell::space_check_validity( )const
{
	ASSERT(!m_elements.empty(),"physics shell space_check_validity fail: no elemnts in shell");
	element_vec::const_iterator		i = m_elements.begin();
	element_vec::const_iterator		e = m_elements.end();
	for( ;i!= e; ++i )
		(*i)->check_collision_validity() ;
}

void shell::set_update_object( xray::physics::update_object_interface *object )
{
	m_update_object = object;
}

void shell::on_step( scheduler &s )
{
	if( !m_update_object )
		return;

	u32 const elements_count = m_elements.size();
	xray::math::float4x4* transforms = (xray::math::float4x4*) ALLOCA( elements_count * sizeof( xray::math::float4x4 ) );
	
	float factor = s.interpolation_factor();

	for( u32 i = 0; i < elements_count; ++i )
		m_elements[i]->interpolate_world_transform( factor, transforms[ i ] );

			//transforms[i] = m_elements[i]->get_geometry( ).get_matrix( );

	m_update_object->physics_update_transform( transforms, elements_count );

}

void shell::deactivate			( xray::physics::world& w )
{
	xray::physics::physics_world * world = static_cast_checked<xray::physics::physics_world*> ( &w );
	
	world->deactivate( *this );

}

void	shell::remove		( scheduler &s )
{
	s.deactivate( this );
}