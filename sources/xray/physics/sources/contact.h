//	Created 	: 10.02.2008
//	Author		: Konstantin Slipchenko
//	Description : contact
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_CONTACT_H_INCLUDED
#define XRAY_PHYSICS_CONTACT_H_INCLUDED
#include "joint_base_impl.h"
#include "contact_ode.h"
#include "ode_include_joint.h"
#include "joint.h"
#include <ode/sources/joints/contact.h>

struct contact_joint_info;

class contact:
	public joint,
	protected joint_base_impl
{
private:
	dxJointContact	m_contact;

public:
	explicit		contact		( const contact_joint_info& c )	;
	explicit		contact		( const contact& c ): m_contact ( c.m_contact ) 	{}
					~contact	( )												{ joint_base_impl::detach( &m_contact ); }

public:
	virtual	void	connect		( body	*b0, body	*b1 )	{ joint_base_impl::connect( &m_contact, b0, b1 ); }
	virtual	void	connect		( body	*b )				{ joint_base_impl::connect( &m_contact, b ); }
	virtual	void	integrate	( float	time_delta )		{ joint_base_impl::integrate( &m_contact, time_delta); }
	virtual	void	fill_island ( island	&i )			{ joint_base_impl::fill_island(&m_contact, i ); }

public:
	virtual	void	render		( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const; //{joint_base_impl::render( &m_contact, renderer );}

private:
	virtual	void				set_anchor  ( const float3	&anchor )	{ XRAY_UNREFERENCED_PARAMETER(anchor); UNREACHABLE_CODE(); }
	virtual	void				get_anchor  ( float3	&anchor )const	{ XRAY_UNREFERENCED_PARAMETER(anchor); UNREACHABLE_CODE(); }

}; // class contact

#endif