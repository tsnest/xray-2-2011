////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.02.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: transformed geometry
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "transformed_geometry.h"

transformed_geometry::transformed_geometry ( geometry * g ): m_geometry( g ) 
{

}

transformed_geometry::~transformed_geometry( )
{

}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og)const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )const
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )const
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )				const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )			const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const cylinder_geometry& og )		const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	transformed_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}
