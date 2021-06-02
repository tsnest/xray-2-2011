////////////////////////////////////////////////////////////////////////////
//	Created		: 10.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_environment.h"
#include "game_world.h"
#include "game.h"
#include <xray/render/world.h>
#include <xray/render/facade/scene_renderer.h>

namespace stalker2{

object_environment::object_environment( game_world& w )
:super			( w )
{
	m_scene_view = m_game_world.get_game().get_render_scene_view( );	
}

object_environment::~object_environment( )
{
}

void object_environment::load( configs::binary_config_value const& t )
{
	super::load					(t);
	pcstr post_effect_name		= pcstr(t["post_effect"]);
	
	resources::query_resource	( post_effect_name, resources::material_class, boost::bind(&object_environment::material_ready, this, _1), g_allocator );
}

void object_environment::material_ready( resources::queries_result& data )
{
	if(data.is_successful())
	{
		m_game_world.get_game().renderer().scene().set_post_process(m_scene_view, data[0].get_unmanaged_resource());
	}
}

void object_environment::unload_contents( )
{
	
}

void object_environment::load_contents( )
{
	
}


} // namespace stalker2