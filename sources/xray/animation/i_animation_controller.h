////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef I_ANIMATION_CONTROLLER_H_INCLUDED
#define I_ANIMATION_CONTROLLER_H_INCLUDED

#include <xray/render/facade/model.h>

namespace xray {
namespace render {

namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

class scene_renderer;

} // namespace render

namespace animation {

namespace mixing {
	class expression;
} // namespace mixing

class i_animation_controller_set;

class i_animation_controller {
public:
	virtual	i_animation_controller_set	const	&action_set				( )const = 0;
	virtual	void								render					(
													render::scene_ptr const& scene,
													render::scene_renderer& scene_renderer,
													render::debug::renderer& debug_renderer,
													render::skeleton_model_ptr const& model,
													float time_global,
													bool render_debug_info
												) const = 0;
	virtual	float4x4							position				()const = 0;
	virtual float								crrent_action_start_time()const = 0;
	virtual	render::skeleton_model_ptr			model					() = 0;

	virtual	pcstr								get_model_path			() = 0;

#ifndef MASTER_GOLD
	virtual	void								set_mixer_callback		( boost::function<void (mixing::expression const&, u32)> const& callback ) = 0;
	virtual	void								clear_mixer				( ) = 0;
	virtual	void								get_dump_string			( string4096 &s ) const = 0;
#endif // #ifndef MASTER_GOLD

	virtual	void								reset					( )= 0;
	virtual	void								init					( u32 id, const buffer_vector< float > &weights, float time_global ) = 0;
	virtual	void								get_bone_world_matrix	( float4x4 &m, u32 bone_id ) const = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_animation_controller )
};  // class i_animation_controller

} // namespace animation
} // namespace xray

#endif // #ifndef I_ANIMATION_CONTROLLER_H_INCLUDED