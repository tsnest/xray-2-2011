////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.02.2008
//	Author		: Konstantin Slipchenko
//	Description : island - integration 
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_ISLAND_H_INCLUDED
#define XRAY_PHYSICS_ISLAND_H_INCLUDED

#include "scheduler.h"
#include "step_counter.h"
#include "integrator_ode.h"
#include "contact.h"
#include "contact_storage.h"

namespace xray {
namespace render{
	class world;
} // namespace render
} // namespace xray

class island_object;
class scheduler;
class step_counter_ref;
class contact;
class contact_storage;

class island:
	private boost::noncopyable
{
	
			scheduler&					m_scheduler;
	const	step_counter_ref			m_step_counter;
			float						m_integration_step;

	vectora< island_object* >&			m_objects;
	contact_storage&					m_contacts;
	integrator_ode						m_integrator;

public:
									island				( scheduler &w, float integration_step );

private:
				void				step				( float integration_step );

public:
				void				step				( );
inline			float				integration_step	( )const	{ return m_integration_step; }
				void				connect				(  island_object* o );
inline			contact&			add_contact			( const contact_joint_info &c );
inline			integrator_ode		&ode				( ){ return m_integrator; }
inline	const	step_counter_ref	&step_counter		( )	const				{ return m_step_counter; }
inline	const	scheduler&			get_scheduler		( )const { return m_scheduler; }
inline			scheduler&			get_scheduler		( )		 { return m_scheduler; }
				void				debug_render		( render::scene_ptr const& scene, render::debug::renderer& renderer ) const;

}; // class island


inline	contact&	island::add_contact			( const contact_joint_info  &c )
{ 
	
	//m_contacts.push_back( NEW(contact)(c) );
	//return *(m_contacts.back());
	//m_contacts.back().fill_island(*this);
	return m_contacts.add( c );
}


#endif
