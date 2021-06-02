////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
//	contact storage
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "contact_storage.h"
#include "contact.h"

contact_storage::contact_storage( )
:m_contacts( xray::physics::g_ph_allocator )
{}

contact& contact_storage::add( const contact_joint_info& c )
{
	m_contacts.push_back( XRAY_NEW_IMPL( xray::physics::g_ph_allocator, contact)(c) );
	return *m_contacts.back();
}

void contact_storage::clear( )
{
	vectora<contact*>::iterator i		= m_contacts.begin();
	vectora<contact*>::const_iterator e = m_contacts.end();
	for( ; i!=e; ++i )
		XRAY_DELETE_IMPL( xray::physics::g_ph_allocator, *i);

	m_contacts.clear();
}

void contact_storage::for_each( contact_predicate &predicate )
{
	vectora<contact*>::iterator i		= m_contacts.begin();
	vectora<contact*>::const_iterator e = m_contacts.end();
	for( ; i!=e; ++i )
		predicate(*(*i));
}


void render( contact_storage &contacts, render::scene_ptr const& scene, render::debug::renderer& renderer )
{
	struct pred:
		public contact_predicate,
		private boost::noncopyable
	{
		render::scene_ptr const& scene;
		render::debug::renderer& renderer;
		pred( xray::render::scene_ptr const& scene, render::debug::renderer& renderer ): scene( scene ), renderer( renderer ) {}
		virtual bool operator() ( contact& c )
		{
			c.render( scene, renderer );
			return true;
		}
	} p( scene, renderer );
	contacts.for_each( p );
}