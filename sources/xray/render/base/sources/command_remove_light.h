////////////////////////////////////////////////////////////////////////////
//	Created		: 23.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_REMOVE_LIGHT_H_INCLUDED
#define COMMAND_REMOVE_LIGHT_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>

namespace xray {
namespace render {

class base_world;

class command_remove_light :
	public engine::command,
	private boost::noncopyable
{
public:
						command_remove_light	( base_world& world, u32 id, bool beditor );
						~command_remove_light	(  );
	virtual	void		execute				( );

private:
	base_world&						m_world;
	u32								m_id;
	bool							m_beditor;
}; // class command_remove_light

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_REMOVE_LIGHT_H_INCLUDED