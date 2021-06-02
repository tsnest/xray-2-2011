// #ifndef COMMAND_REMOVE_VISUAL_H_INCLUDED////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_REMOVE_VISUAL_H_INCLUDED
#define COMMAND_REMOVE_VISUAL_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>

namespace xray {
namespace render {

class base_world;

class command_remove_visual :
	public engine::command,
	private boost::noncopyable
{
public:
						command_remove_visual	( base_world& world, u32 id );
						~command_remove_visual	(  );
	virtual	void		execute				( );

private:
	base_world&						m_world;
	u32									m_id;
}; // class command_remove_visual

} // namespace render
} // namespace xray

#endif  // #ifndef COMMAND_REMOVE_VISUAL_H_INCLUDED