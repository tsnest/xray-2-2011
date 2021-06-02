////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMTION_WORLD_H_INCLUDED
#define ANIMTION_WORLD_H_INCLUDED

#include <xray/animation/world.h>
#include <xray/animation/bi_spline_data.h>
#include "bi_spline_skeleton_animation.h"
#include <xray/animation/skeleton_animation.h>

namespace xray {

namespace render {
	class scene_renderer;
} // namespace render

namespace configs{
	class lua_config_value;
} // namespace configs

namespace animation {

struct engine;
class bi_spline_bone_animation;
class bi_spline_skeleton_animation;
class skeleton;
class skeleton_animation;

class animation_world:
	public animation::world,
	private boost::noncopyable
{
public:
					animation_world		(
						animation::engine& engine,
						render::scene_ptr const& test_scene,
						render::scene_renderer* scene_renderer,
						render::debug::renderer* debug_renderer
					);
	virtual			~animation_world	( );
	virtual	void	clear_resources		( );
	virtual	void	tick				( );
	virtual void	set_test_scene		( render::scene_ptr const& scene );
	virtual bi_spline_skeleton_animation* create_skeleton_animation_data	( u32 bones_count );
	virtual	void	destroy				( bi_spline_skeleton_animation* &p );

private:
	engine&								m_engine;
	render::scene_renderer*				m_scene_renderer;
	render::debug::renderer*			m_debug_renderer;
	render::scene_ptr					m_test_scene;
}; // class world

} // namespace animation
} // namespace xray

#endif // #ifndef ANIMTION_WORLD_H_INCLUDED