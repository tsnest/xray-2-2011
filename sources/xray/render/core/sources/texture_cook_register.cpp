////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_cook_wrapper.h"
#include "texture_converter_cook.h"
#include "texture_cook.h"

namespace xray {
namespace render {

void register_texture_cook	( );
void unregister_texture_cook( );

} // namespace render
} // namespace xray

static xray::uninitialized_reference< xray::render::texture_cook_wrapper >	s_texture_wrapper_cook;
static xray::uninitialized_reference< xray::render::texture_options_binary_cooker >	s_texture_options_binary_cooker;

#ifndef MASTER_GOLD
	static xray::uninitialized_reference< xray::render::texture_converter_cook >	s_texture_converter_cook;
	static xray::uninitialized_reference< xray::render::texture_options_lua_cooker>	s_texture_options_lua_cooker;
#endif // #ifndef MASTER_GOLD

static xray::uninitialized_reference< xray::render::texture_cook >			s_texture_cook;

void xray::render::register_texture_cook	( )
{
	XRAY_CONSTRUCT_REFERENCE	( s_texture_wrapper_cook, texture_cook_wrapper );
	resources::register_cook	( s_texture_wrapper_cook.c_ptr() );

#ifndef MASTER_GOLD
	XRAY_CONSTRUCT_REFERENCE	( s_texture_converter_cook, texture_converter_cook );
	resources::register_cook	( s_texture_converter_cook.c_ptr() );

	XRAY_CONSTRUCT_REFERENCE	( s_texture_options_lua_cooker, texture_options_lua_cooker );
	resources::register_cook	( s_texture_options_lua_cooker.c_ptr() );
#endif // #ifndef MASTER_GOLD

	XRAY_CONSTRUCT_REFERENCE	( s_texture_cook, texture_cook );
	resources::register_cook	( s_texture_cook.c_ptr() );

	XRAY_CONSTRUCT_REFERENCE	( s_texture_options_binary_cooker, texture_options_binary_cooker );
	resources::register_cook	( s_texture_options_binary_cooker.c_ptr() );
}

void xray::render::unregister_texture_cook	( )
{
	resources::unregister_cook	( resources::texture_wrapper_class );
	XRAY_DESTROY_REFERENCE		( s_texture_wrapper_cook );

#ifndef MASTER_GOLD
	resources::unregister_cook	( resources::texture_converter_class );
	XRAY_DESTROY_REFERENCE		( s_texture_converter_cook );
#endif // #ifndef MASTER_GOLD

	resources::unregister_cook	( resources::texture_class );
	XRAY_DESTROY_REFERENCE		( s_texture_cook );
}