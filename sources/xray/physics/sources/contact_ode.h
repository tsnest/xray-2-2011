////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.02.2008
//	Author		: Konstantin Slipchenko
//	Description : contact ode
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_CONTACT_ODE_H_INCLUDED
#define XRAY_PHYSICS_CONTACT_ODE_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

enum debug_user_id;

} // namespace render
} // namespace xray

//#include "joint_ode_base.h"
//#include "ode_include_joint.h"

struct contact_joint_info;

//class contact_ode//:
//	//public joint_ode_base
//{
//	dxJointContact	m_contact;
//
//public:
//	explicit	contact_ode( const contact_joint_info &c );
//				contact_ode( const contact_ode &c ) { m_contact = c.m_contact; }
//
////	virtual			dxJoint*	dx_joint	()						{ return &m_contact; }
////	virtual	const	dxJoint*	dx_joint	()const					{ return &m_contact; };
//public:
//	virtual					~contact_ode( ){}
//	virtual		void		integrate	( float	time_delta )	;
//	virtual		void		render		( render::debug::renderer& renderer ) const;			
//
//};

struct dxJointContact;
void	init_contact_ode( dxJointContact	&contact, const contact_joint_info &c );
void	render( const dxJointContact	&contact, render::scene_ptr const& scene, render::debug::renderer& renderer );
#endif