////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_DECAL_H_INCLUDED
#define OBJECT_DECAL_H_INCLUDED

#include "object.h"
#include <xray/render/facade/game_renderer.h>

namespace xray{
	namespace render{
		struct material_effects_instance_cook_data;
	}
}
namespace stalker2{

class object_decal: public game_object_static
{
	typedef game_object_static				super;
public:
					object_decal			(game_world& w);
					~object_decal			();
	virtual void	load					(configs::binary_config_value const& t);
			void	requery_material		();
			void	material_ready			(resources::queries_result& data, xray::render::material_effects_instance_cook_data* cook_data);
	
			render::scene_ptr				m_scene;
			u32								m_decal_id;
			float							m_alpha_angle;
			float							m_clip_angle;
			bool							m_projection_on_static_geometry;
			bool							m_projection_on_skeleton_geometry;
			bool							m_projection_on_terrain_geometry;
			bool							m_projection_on_speedtree_geometry;
			bool							m_projection_on_particle_geometry;
	
			float							m_decal_far_distance;
			float							m_decal_width;
			float							m_decal_height;
}; // class object_decal

} // namespace stalker2

#endif // #ifndef OBJECT_DECAL_H_INCLUDED