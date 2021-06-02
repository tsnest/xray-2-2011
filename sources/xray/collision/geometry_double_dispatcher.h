////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRY_DOUBLE_DISPATCHER_H_INCLUDED
#define GEOMETRY_DOUBLE_DISPATCHER_H_INCLUDED

namespace xray
{
	namespace collision
	{
		class	geometry_instance;
		class	box_geometry_instance;
		class	sphere_geometry_instance;
		class	cylinder_geometry_instance;
		class	capsule_geometry_instance;
		class	truncated_sphere_geometry_instance;

		struct XRAY_NOVTABLE geometry_double_dispatcher
		{
			virtual	void	dispatch	( box_geometry_instance const& left,					box_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( box_geometry_instance const& left,					sphere_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( box_geometry_instance const& left,					cylinder_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( box_geometry_instance const& left,					capsule_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( box_geometry_instance const& left,					truncated_sphere_geometry_instance const& right ) = 0;

			virtual	void	dispatch	( sphere_geometry_instance const& left,				box_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( sphere_geometry_instance const& left,				sphere_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( sphere_geometry_instance const& left,				cylinder_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( sphere_geometry_instance const& left,				capsule_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( sphere_geometry_instance const& left,				truncated_sphere_geometry_instance const& right ) = 0;

			virtual	void	dispatch	( cylinder_geometry_instance const& left,			box_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( cylinder_geometry_instance const& left,			sphere_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( cylinder_geometry_instance const& left,			cylinder_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( cylinder_geometry_instance const& left,			capsule_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( cylinder_geometry_instance const& left,			truncated_sphere_geometry_instance const& right ) = 0;

			virtual	void	dispatch	( capsule_geometry_instance const& left,				box_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( capsule_geometry_instance const& left,				sphere_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( capsule_geometry_instance const& left,				cylinder_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( capsule_geometry_instance const& left,				capsule_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( capsule_geometry_instance const& left,				truncated_sphere_geometry_instance const& right ) = 0;

			virtual	void	dispatch	( truncated_sphere_geometry_instance const& left,	box_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& left,	sphere_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& left,	cylinder_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& left,	capsule_geometry_instance const& right ) = 0;
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& left,	truncated_sphere_geometry_instance const& right ) = 0;

		protected:
			XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( geometry_double_dispatcher )
		}; // class geometry_double_dispatcher
	} // namespace collision
} // namespace xray

#endif // #ifndef GEOMETRY_DOUBLE_DISPATCHER_H_INCLUDED