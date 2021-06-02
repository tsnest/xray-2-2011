////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.02.2008
//	Author		: Konstantin Slipchenko
//	Description : contact ode
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "contact_ode.h"
#include "ode_include.h"
#include "ode_convert.h"
#include "integration_constants.h"
#include "contact_joint_info.h"
#include "ode_include_joint.h"
#include <xray/render/facade/debug_renderer.h>

inline static void	d_init_surface( dSurfaceParameters &surface, const contact_joint_info &c )
{
		surface.mu			= 1.f;// 5000.f;
		surface.soft_erp	= 1.f;//ERP(world_spring,world_damping);
		surface.soft_cfm	= 1.f;//CFM(world_spring,world_damping);
		surface.bounce		= 0.01f;//0.1f;
		surface.bounce_vel	= 1.5f;//0.005f;


		surface.mode =dContactApprox1|dContactSoftERP|dContactSoftCFM;

		float spring	= /*material_2->fPHSpring*material_1->fPHSpring*/default_spring;
		float damping	=/*material_2->fPHDamping*material_1->fPHDamping*/default_damping;
		surface.soft_erp=	erp( spring, damping, c.integration_step );
		surface.soft_cfm=	cfm( spring, damping, c.integration_step );

		surface.mu=1.f;/*material_2->fPHFriction*material_1->fPHFriction;*/

		//if(flags_1.test(SGameMtl::flBounceable)&&flags_2.test(SGameMtl::flBounceable))
		//{
		//	surface.mode		|=	dContactBounce;
		//	surface.bounce_vel	=	_max(material_1->fPHBounceStartVelocity,material_2->fPHBounceStartVelocity);
		//	surface.bounce		=	_min(material_1->fPHBouncing,material_2->fPHBouncing);
		//}
}

inline static void	d_init_contact_geom( dContactGeom &geom, const xray::collision::contact_info &c )
{
	geom.depth				= c.depth;
	geom.g1					= 0;
	geom.g2					= 0;
	cast_xr ( geom.normal ) = c.normal;
	cast_xr ( geom.pos )	= c.position;
}

inline static void	d_init_contact( dContact &dc, const contact_joint_info &c )
{
	cast_xr( dc.fdir1 )	= float3(0,1,0);
	d_init_contact_geom( dc.geom, c.c );
	d_init_surface( dc.surface, c );

}
///////////////////////////////////////////////////////////////////////////////////////////////
void	init_contact_ode( dxJointContact	&contact, const contact_joint_info &c ) 
{
	dContact dc;
	d_init_contact( dc, c );
	dJointInitContact( &contact, &dc );
}

//void	contact_ode::integrate( float	time_delta )
//{
//	
//}

void	render( const dxJointContact	&contact, render::scene_ptr const& scene, render::debug::renderer& renderer )
{
	
	const float scale = 1;
	const float3 &p0 = cast_xr(contact.contact.geom.pos);
	const float3 &p1 = cast_xr(contact.contact.geom.pos)+cast_xr(contact.contact.geom.normal)*contact.contact.geom.depth*scale;
	renderer.draw_sphere( scene, p0, 0.01f, math::color( 255u, 255u, 0u, 255u ) );
	//world.debug(user_id).draw_arrow( p0, p1, math::color_xrgb( 255, 0, 0 ) );

	renderer.draw_line( scene, p0, p1, math::color( 255u, 0u, 0u, 255u ) );
}

