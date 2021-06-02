////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2008
//	Author		: Konstantin Slipchenko
//	Description : geometry: collision geometry to integration cycle interface
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PHYSICS_GEOMETRY_H_INCLUDED
#define XRAY_PHYSICS_GEOMETRY_H_INCLUDED
#include "pose_anchor.h"

class contact_info_buffer;

class complex_geometry;
class transformed_geometry;

class box_geometry;
class sphere_geometry;
class cylinder_geometry;
class triangle_mesh_base;

class geometry
{

public:
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const geometry& og )					const = 0;

	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const transformed_geometry& og )		const = 0;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const complex_geometry& og )			const = 0;

	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const box_geometry& og )				const = 0;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const sphere_geometry& og )			const = 0;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const cylinder_geometry& og )		const = 0;
	virtual		void	generate_contacts( contact_info_buffer& contacts, const pose_anchor &anch, const triangle_mesh_base& og )	const = 0;
public:
	virtual				~geometry( ){}
protected:
//	virtual	void	generate_contacts( contact_info_buffer	&contacts, const pose_anchor &anch, const geometry	&og )const =0;
};

#endif