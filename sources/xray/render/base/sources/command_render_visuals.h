////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_RENDER_VISUALS_H_INCLUDED
#define COMMAND_RENDER_VISUALS_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>

namespace xray {
namespace render {

class base_world;

class command_render_visuals :
	public engine::command,
	private boost::noncopyable
{
public:
							command_render_visuals	( base_world& world );
							~command_render_visuals	(  );
	virtual	void			execute				( );

private:
	base_world&						m_world;
}; // class command_render_visuals

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_RENDER_VISUALS_H_INCLUDED