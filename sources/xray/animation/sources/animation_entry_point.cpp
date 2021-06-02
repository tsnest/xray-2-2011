////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_world.h"
#include <xray/animation/api.h>
#include <xray/linkage_helper.h>
#include "mixing_binary_tree_writer.h"

#ifndef MASTER_GOLD
#	include "editor_animation.h"
#	include "editor_mixer.h"
#endif // #ifndef MASTER_GOLD

XRAY_DECLARE_LINKAGE_ID(animation_entry_point)

using xray::animation::animation_world;

static xray::uninitialized_reference< animation_world >	s_world;
xray::animation::allocator_type*	xray::animation::g_allocator = 0;

namespace xray {
namespace animation {

static bool is_enum_exported = false;

#ifndef MASTER_GOLD
static configs::enum_tuple animation_tuples[] =
{
	std::make_pair( "single_animation_type",		resources::single_animation_class	  ),
	std::make_pair( "composite_animation_type",		resources::composite_animation_class  ),
	std::make_pair( "collection_animation_type",	resources::animation_collection_class )
};
#endif // #ifndef MASTER_GOLD

} // namespace animation
} // namespace xray

xray::animation::world* xray::animation::create_world(
		engine& engine,
		render::scene_ptr const& test_scene,
		render::scene_renderer* scene_renderer,
		render::debug::renderer* debug_renderer
	)
{
#ifndef MASTER_GOLD	
	if ( !xray::animation::is_enum_exported )
	{
		xray::configs::export_enum( "animation_resources", xray::animation::animation_tuples, xray::animation::animation_tuples + array_size( xray::animation::animation_tuples ) );
		xray::animation::is_enum_exported = true;
	}
#endif // #ifndef MASTER_GOLD

	XRAY_CONSTRUCT_REFERENCE	( s_world, animation_world )( engine, test_scene, scene_renderer, debug_renderer );
	return						( s_world.c_ptr() );
}

void xray::animation::destroy_world( xray::animation::world*& world )
{
	R_ASSERT					( s_world.c_ptr() == world );
	XRAY_DESTROY_REFERENCE		( s_world );
	world						= 0;
}

void xray::animation::set_memory_allocator( allocator_type& allocator )
{
	ASSERT						( !g_allocator );
	g_allocator					= &allocator;
}


#ifndef MASTER_GOLD
xray::animation::i_editor_animation* xray::animation::create_editor_animation( allocator_type& allocator, skeleton_animation_ptr const& anim )
{
	return XRAY_NEW_IMPL( allocator, editor_animation )( anim );
}

xray::animation::i_editor_animation* xray::animation::create_editor_animation( allocator_type& allocator, pcstr anim_file )
{
	editor_animation* anim = XRAY_NEW_IMPL(allocator, editor_animation)( anim_file );
	if (!anim->is_exist())
	{
		//destroy_editor_animation(allocator, anim);
		XRAY_DELETE_IMPL( allocator, anim );
		return 0;
	}
	return anim;
}

xray::animation::i_editor_animation* xray::animation::create_editor_animation( pcstr anim_file )
{
	return create_editor_animation( *g_allocator, anim_file );
}

void xray::animation::destroy_editor_animation(  i_editor_animation* &anim )
{
	destroy_editor_animation( *g_allocator, anim );
}

void xray::animation::destroy_editor_animation( allocator_type& allocator, i_editor_animation* &anim )
{
	xray::animation::editor_animation* a = static_cast<xray::animation::editor_animation*>(anim);
	XRAY_DELETE_IMPL( allocator, a );
	anim = 0;
}

xray::animation::i_editor_mixer* xray::animation::create_editor_mixer( allocator_type& allocator, animation_player& animation_player )
{
	return XRAY_NEW_IMPL( allocator, editor_mixer )( allocator, animation_player );
}

void xray::animation::destroy_editor_mixer( allocator_type& allocator, i_editor_mixer* mxr )
{
	xray::animation::editor_mixer* m = static_cast<xray::animation::editor_mixer*>(mxr);
	XRAY_DELETE_IMPL( allocator, m );
}

xray::animation::mixing::binary_tree_visitor* xray::animation::create_binary_tree_writer( allocator_type& allocator, xray::configs::lua_config_value& cfg, float2& pos )
{
	return XRAY_NEW_IMPL(allocator, xray::animation::mixing::binary_tree_writer)(cfg, pos);
}

void xray::animation::destroy_binary_tree_writer( allocator_type& allocator, xray::animation::mixing::binary_tree_visitor*& wr )
{
	xray::animation::mixing::binary_tree_writer* bwr = static_cast<xray::animation::mixing::binary_tree_writer*>(wr);
	XRAY_DELETE_IMPL(allocator, bwr);
	wr = 0;
}

#endif // #ifndef MASTER_GOLD