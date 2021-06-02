////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.02.2008
//	Author		: Konstantin Slipchenko
//	Description : box
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_BOX_GEOMETRY_H_INCLUDED
#define XRAY_PHYSICS_BOX_GEOMETRY_H_INCLUDED
#include "geometry.h"
class box_geometry:
	public geometry
{
	float3		m_box;

	
//public:
//				void	set( const float3& box ) { m_box = box; }
public:
	explicit	box_geometry ( const float3 &b )	{ m_box = b; }
	virtual		~box_geometry( )					{}
private:
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og )					const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )		const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )			const ;
private:	

	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )				const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )			const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const cylinder_geometry& og )		const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const ;

};
#endif