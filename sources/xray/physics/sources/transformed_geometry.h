////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.02.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: transformed geometry
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_TRANSFORMED_GEOMETRY_H_INCLUDED
#define XRAY_PHYSICS_TRANSFORMED_GEOMETRY_H_INCLUDED
#include "geometry.h"
#include "pose_anchor.h"

class transformed_geometry:
	public geometry
{
	
	geometry		*m_geometry;
	pose_anchor		m_pose;
	

public:
	explicit	transformed_geometry ( geometry	*g );
	virtual		~transformed_geometry( );
private:
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og )					const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )		const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )			const ;
	

	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )				const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )			const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const cylinder_geometry& og )		const ;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const ;

};
#endif

