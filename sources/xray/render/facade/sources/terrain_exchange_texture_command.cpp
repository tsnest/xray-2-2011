////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_exchange_texture_command.h"
#include <xray/render/world.h>
#include <xray/render/engine/world.h>
#include <xray/memory_macros.h>

namespace xray {
namespace render {

terrain_exchange_texture_command::terrain_exchange_texture_command	(
		engine::world& render_engine_world,
		scene_ptr const& in_scene,
		fs_new::virtual_path_string const& old_texture, 
		fs_new::virtual_path_string const& new_texture
	) :
	m_render_engine_world	( render_engine_world ),
	m_scene_ptr				( in_scene ),
	m_old					( old_texture ),
	m_new					( new_texture )
{
}

void terrain_exchange_texture_command::execute						( )
{
	m_render_engine_world.terrain_exchange_texture	( m_scene_ptr, m_old, m_new );
}

} // namespace render
} // namespace xray