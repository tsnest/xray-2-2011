////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/world.h>
#include <xray/render/engine/world.h>
#include <xray/render/facade/model.h>
#include "update_skeleton_command.h"

namespace xray {
namespace render {

update_skeleton_command::update_skeleton_command(
		engine::world& world, 
		render_model_instance_ptr instance, 
		math::float4x4 const* matrices, u32 count
	) :
	m_world					( world ),
	m_model_instance		( instance ),
	m_matrices_count		( count )
{
	u32 const buff_size		= sizeof(math::float4x4)*count;
	memory::copy			( m_matrices, buff_size, matrices, buff_size);
}

void update_skeleton_command::execute( )
{
	m_world.update_skeleton	( m_model_instance, m_matrices, m_matrices_count );
}

} // namespace render
} // namespace xray