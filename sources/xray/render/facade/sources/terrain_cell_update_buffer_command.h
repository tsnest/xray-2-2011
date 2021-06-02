////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TERRAIN_CELL_UPDATE_BUFFER_H_INCLUDED
#define COMMAND_TERRAIN_CELL_UPDATE_BUFFER_H_INCLUDED

#include <xray/render/facade/base_command.h>
#include <xray/render/facade/common_types.h>
#include <xray/render/facade/model.h>
#include <xray/render/engine/base_classes.h>
#include <xray/render/facade/editor_renderer.h>

namespace xray {
namespace render {

class world;

class terrain_cell_update_buffer_command :
	public base_command,
	private boost::noncopyable
{
public:
	typedef render::editor::on_command_finished_functor_type	on_command_finished_functor_type;

public:
					terrain_cell_update_buffer_command	(
						render::engine::world& render_engine_world,
						scene_ptr const& in_scene,
						render_model_instance_ptr const& v, 
						xray::vectora< terrain_buffer_fragment >& fragments,
						float4x4 const& transform,
						on_command_finished_functor_type const& on_command_finished_functor
					);
	virtual			~terrain_cell_update_buffer_command	( );
	virtual	void	execute								( );

private:
	vectora<terrain_buffer_fragment>	m_fragments;
	math::float4x4						m_transform;
	on_command_finished_functor_type	m_on_command_finished_functor;
	scene_ptr							m_scene_ptr;
	render_model_instance_ptr		m_model;
	render::engine::world&				m_render_engine_world;
}; // class terrain_cell_update_buffer_command

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_TERRAIN_CELL_UPDATE_BUFFER_H_INCLUDED
