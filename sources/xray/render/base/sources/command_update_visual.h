////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_UPDATE_VISUAL_H_INCLUDED
#define COMMAND_UPDATE_VISUAL_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>

namespace xray {
namespace render {

class base_world;

class command_update_visual :
	public engine::command,
	private boost::noncopyable
{
public:
							command_update_visual	( base_world& world, u32 id, math::float4x4 const& transform, bool selected = false );
							~command_update_visual	(  );
	virtual	void			execute				( );

private:
	math::float4x4						m_transform;
	base_world&						m_world;
	u32									m_id;
	bool								m_selected;
}; // class command_update_visual

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_UPDATE_VISUAL_H_INCLUDED