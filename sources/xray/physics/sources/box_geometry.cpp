////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: box geometry
////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "box_geometry.h"

void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og )	const
{
	og.generate_contacts( contacts, anch, *this );
}

void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )		const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);

}
void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )			const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
	//og.generate_contacts( contacts, anch, *this );
}

void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )				const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
	//
}

void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )			const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
	//
}
void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const cylinder_geometry& og )		const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}

void	box_geometry::generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&contacts, anch, &og);
}