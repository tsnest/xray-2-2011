////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_UPDATE_SKELETON_H_INCLUDED
#define COMMAND_UPDATE_SKELETON_H_INCLUDED

#include <xray/render/facade/base_command.h>

namespace xray {
namespace render {

class update_skeleton_command :
	public base_command,
	private boost::noncopyable
{
public:
							update_skeleton_command		(	engine::world& world, 
															render_model_instance_ptr instance, 
															math::float4x4 const* matrices, 
															u32 count );
	virtual	void			execute						( );

private:
	math::float4x4					m_matrices[80];
	render_model_instance_ptr	m_model_instance;
	engine::world&					m_world;
	u32								m_matrices_count;
}; // class update_skeleton_command

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_UPDATE_SKELETON_H_INCLUDED