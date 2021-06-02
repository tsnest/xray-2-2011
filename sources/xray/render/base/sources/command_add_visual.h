////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_ADD_VISUAL_H_INCLUDED
#define COMMAND_ADD_VISUAL_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/visual.h>

namespace xray {
namespace render {

class base_world;

class command_add_visual :
	public engine::command,
	private boost::noncopyable
{
public:
							command_add_visual	(	base_world& world, 
													u32 id, 
													xray::render::visual_ptr const& v, 
													math::float4x4 const& transform, 
													bool selected /*= false*/, 
													bool system_object /*= false*/, 
													bool beditor_chain );
	virtual	void			execute				( );

private:
	math::float4x4					m_transform;
	xray::render::visual_ptr		m_object;
	u32								m_id;
	base_world&						m_world;
	bool							m_selected;
	bool							m_system;
	bool							m_beditor;
}; // class command_add_visual

} // namespace render
} // namespace xray

#endif // COMMAND_ADD_VISUAL_H_INCLUDED