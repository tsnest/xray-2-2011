////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD

#include "terrain_cell_update_buffer_command.h"

#include <xray/render/world.h>
#include <xray/render/engine/world.h>

namespace xray{
namespace render{

terrain_cell_update_buffer_command::terrain_cell_update_buffer_command	(
		render::engine::world& render_engine_world,
		scene_ptr const& in_scene,
		render_model_instance_ptr const& v, 
		vectora<terrain_buffer_fragment> & fragments,
		math::float4x4 const& transform,
		render::editor::on_command_finished_functor_type const& on_command_finished_functor
	) : 
	m_render_engine_world			( render_engine_world ),
	m_scene_ptr						( in_scene ),
	m_model							( v ),
	m_fragments						( fragments.get_allocator() ),
	m_transform						( transform ),
	m_on_command_finished_functor	( on_command_finished_functor )
{
	m_fragments.swap				( fragments );
	R_ASSERT						( v );
}

terrain_cell_update_buffer_command::~terrain_cell_update_buffer_command	()
{
	if ( m_on_command_finished_functor )
		m_on_command_finished_functor	( );
}

void terrain_cell_update_buffer_command::execute()
{
	m_render_engine_world.terrain_update_cell_buffer( m_scene_ptr, m_model, m_fragments, m_transform );
}


} // namespace render
} // namespace xray

#endif // #ifndef MASTER_GOLD