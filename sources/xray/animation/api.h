////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_API_H_INCLUDED
#define XRAY_ANIMATION_API_H_INCLUDED

#include <xray/animation/type_definitions.h>
#include <xray/render/engine/base_classes.h>

#ifndef XRAY_ANIMATION_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_ANIMATION_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_ANIMATION_BUILDING
#			define XRAY_ANIMATION_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_ANIMATION_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_ANIMATION_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_ANIMATION_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_ANIMATION_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_ANIMATION_API

namespace xray {

namespace render {

class scene_renderer;

namespace debug {
	class renderer;
} // namespace game

} // namespace render

namespace animation {

struct engine;
struct world;

#ifndef MASTER_GOLD

class skeleton_animation;
struct i_editor_animation;
class i_editor_mixer;
class skeleton;
class animation_player;

namespace mixing {
	struct binary_tree_visitor;
} // namespace mixing

typedef	resources::resource_ptr <
	animation_player,
	resources::unmanaged_intrusive_base
> animation_player_ptr;

#endif // #ifndef MASTER_GOLD

typedef memory::doug_lea_allocator_type				allocator_type;

XRAY_ANIMATION_API	world*							create_world				(
														engine& engine,
														render::scene_ptr const& test_scene,
														render::scene_renderer* scene_renderer,
														render::debug::renderer* debug_renderer
													);
XRAY_ANIMATION_API	void							destroy_world				( world*& world );
XRAY_ANIMATION_API	void							set_memory_allocator		( allocator_type& allocator );

#ifndef MASTER_GOLD
XRAY_ANIMATION_API	i_editor_animation*				create_editor_animation		( allocator_type& allocator, skeleton_animation_ptr const& anim );
XRAY_ANIMATION_API	i_editor_animation*				create_editor_animation		( allocator_type& allocator, pcstr anim_file );
XRAY_ANIMATION_API	void							destroy_editor_animation	( allocator_type& allocator, i_editor_animation* &anim );

XRAY_ANIMATION_API	i_editor_animation*				create_editor_animation		( pcstr anim_file );
XRAY_ANIMATION_API	void							destroy_editor_animation	( i_editor_animation* &anim );

XRAY_ANIMATION_API	i_editor_mixer*					create_editor_mixer			( allocator_type& allocator, animation_player& animation_player );
XRAY_ANIMATION_API	void							destroy_editor_mixer		( allocator_type& allocator, i_editor_mixer* mxr );

XRAY_ANIMATION_API	mixing::binary_tree_visitor*	create_binary_tree_writer	( allocator_type& allocator, xray::configs::lua_config_value& cfg, float2& pos );
XRAY_ANIMATION_API	void							destroy_binary_tree_writer	( allocator_type& allocator, mixing::binary_tree_visitor*& wr );
#endif // #ifndef MASTER_GOLD

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_API_H_INCLUDED