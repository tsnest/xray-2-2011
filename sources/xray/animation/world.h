////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_WORLD_H_INCLUDED
#define XRAY_ANIMATION_WORLD_H_INCLUDED

namespace xray {

namespace input{
	struct world;
} // namespace input

namespace render {
namespace debug {

	class renderer;

} // namespace debug 

namespace game {

	class renderer;

} // namespace game

} // namespace render

namespace animation {

struct handler;
class bi_spline_skeleton_animation;
class  i_animation_controller_set;
class  i_animation_controller;

struct XRAY_NOVTABLE world {

	virtual	void						clear_resources					( ) = 0;
	virtual	void						tick							( ) = 0;
	
	virtual void						set_test_scene					( render::scene_ptr const& scene ) = 0;

	virtual bi_spline_skeleton_animation* create_skeleton_animation_data( u32 bones_count ) = 0;
	virtual	void						destroy							( bi_spline_skeleton_animation	* &p ) =0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // class world

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_WORLD_H_INCLUDED