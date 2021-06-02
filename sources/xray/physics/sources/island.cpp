////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.02.2008
//	Author		: Konstantin Slipchenko
//	Description : island - integration 
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "island.h"
#include "island_object.h"
#include "contact.h"
#include "physics_world.h"
#include "temp_storage_holder.h"

island::island( scheduler &w, float integration_step )
:m_scheduler		( w ), 
m_step_counter		( w.step_counter() ), 
m_integration_step	( integration_step ),
m_objects			( w.world().get_temp_storage_holder().island_objects_pointers	( ) ),
m_contacts			( w.world().get_temp_storage_holder().contacts() ),
m_integrator		( w.world().get_temp_storage_holder().joints(),w.world().get_temp_storage_holder().bodies() )
{
	//m_contacts.clear();
}

void	island::step( )
{
		step( m_integration_step );
}

void	island::step( float integration_step )
{
	{
		vectora<island_object*>::iterator		i = m_objects.begin();
		vectora<island_object*>::const_iterator	e = m_objects.end();
		for( ; i!=e ;++i )
			(*i)->island_step_start( *this,  integration_step );
	}
	
	m_integrator.step( integration_step );

	//{
	//	physics::vector<contact*>::iterator			i = m_contacts.begin();
	//	physics::vector<contact*>::const_iterator	e = m_contacts.end();
	//	for( ; i!=e ;++i )
	//	{
	//		(*i)->integrate( integration_step );
	//		DELETE((*i));
	//		m_contacts.clear();
	//	}

	//}
//	render( scene );
	m_contacts.clear();

	{
		vectora<island_object*>::iterator		i = m_objects.begin();
		vectora<island_object*>::const_iterator	e = m_objects.end();
		for( ; i!=e ;++i )
			(*i)->island_step_end( *this,  integration_step );
	}

	//{
	//	physics::vector<island_object*>::iterator		i = m_objects.begin();
	//	physics::vector<island_object*>::const_iterator	e = m_objects.end();
	//	for( ; i!=e ;++i )
	//		(*i)->position_integrate( integration_step );
	//}

	m_objects.clear();

}

void	island::connect( island_object* o )
{
	
	ASSERT( o );
	m_objects.push_back( o );
	o->on_island_connect( *this );
}



void island::debug_render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	::render( m_contacts, scene, renderer );
	
	vectora< island_object* >::iterator i = m_objects.begin();
	vectora< island_object* >::const_iterator e = m_objects.end();
	for( ; i != e; ++i )
		(*i)->render( scene, renderer );

}
