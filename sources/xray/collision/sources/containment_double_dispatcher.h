////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_CONTAINMENT_DOUBLE_DISPATCHER_H_INCLUDED
#define XRAY_COLLISION_CONTAINMENT_DOUBLE_DISPATCHER_H_INCLUDED

#include <xray/collision/geometry_double_dispatcher.h>

#ifndef MASTER_GOLD
#	include <xray/render/facade/debug_renderer.h>
#endif

namespace xray
{
	namespace collision
	{
		class geometry_instance;
		class box_geometry_instance;
		class sphere_geometry_instance;
		class capsule_geometry_instance;
		class cylinder_geometry_instance;
		class truncated_sphere_geometry_instance;

		struct containment_double_dispatcher: public geometry_double_dispatcher, private boost::noncopyable
		{
			containment_double_dispatcher(
				geometry_instance const& bounding_volume,
				geometry_instance const& testee
			);

#ifndef MASTER_GOLD
			containment_double_dispatcher(
				geometry_instance const& bounding_volume,
				geometry_instance const& testee,
				render::debug::renderer* renderer,
				render::scene_ptr const* scene
			);
#endif

			bool			check_contains	( );
			
			virtual	void	dispatch	( box_geometry_instance const& bounding_volume,				box_geometry_instance const& testee );
			virtual	void	dispatch	( box_geometry_instance const& bounding_volume,				sphere_geometry_instance const& testee );
			virtual	void	dispatch	( box_geometry_instance const& bounding_volume,				cylinder_geometry_instance const& testee );
			virtual	void	dispatch	( box_geometry_instance const& bounding_volume,				capsule_geometry_instance const& testee );
			virtual	void	dispatch	( box_geometry_instance const& bounding_volume,				truncated_sphere_geometry_instance const& testee );

			virtual	void	dispatch	( sphere_geometry_instance const& bounding_volume,			box_geometry_instance const& testee );
			virtual	void	dispatch	( sphere_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee );
			virtual	void	dispatch	( sphere_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee );
			virtual	void	dispatch	( sphere_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee );
			virtual	void	dispatch	( sphere_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee );

			virtual	void	dispatch	( cylinder_geometry_instance const& bounding_volume,			box_geometry_instance const& testee );
			virtual	void	dispatch	( cylinder_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee );
			virtual	void	dispatch	( cylinder_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee );
			virtual	void	dispatch	( cylinder_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee );
			virtual	void	dispatch	( cylinder_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee );

			virtual	void	dispatch	( capsule_geometry_instance const& bounding_volume,			box_geometry_instance const& testee );
			virtual	void	dispatch	( capsule_geometry_instance const& bounding_volume,			sphere_geometry_instance const& testee );
			virtual	void	dispatch	( capsule_geometry_instance const& bounding_volume,			cylinder_geometry_instance const& testee );
			virtual	void	dispatch	( capsule_geometry_instance const& bounding_volume,			capsule_geometry_instance const& testee );
			virtual	void	dispatch	( capsule_geometry_instance const& bounding_volume,			truncated_sphere_geometry_instance const& testee );

			virtual	void	dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	box_geometry_instance const& testee );
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	sphere_geometry_instance const& testee );
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	cylinder_geometry_instance const& testee );
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	capsule_geometry_instance const& testee );
			virtual	void	dispatch	( truncated_sphere_geometry_instance const& bounding_volume,	truncated_sphere_geometry_instance const& testee );

		private:
			geometry_instance const&	m_bounding_volume;
			geometry_instance const&	m_testee;
			bool						m_result;

		//debug
#ifndef MASTER_GOLD
		private:
			render::debug::renderer*	m_renderer;
			render::scene_ptr const*	m_scene;
#endif

		}; // class containment_double_dispatcher
	} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_CONTAINMENT_DOUBLE_DISPATCHER_H_INCLUDED