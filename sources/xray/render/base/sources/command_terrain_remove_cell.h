////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TERRAIN_REMOVE_CELL_H_INCLUDED
#define COMMAND_TERRAIN_REMOVE_CELL_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/visual.h>

namespace xray {
namespace render {

class base_world;

class command_terrain_remove_cell :
	public engine::command,
	private boost::noncopyable
{
public:
							command_terrain_remove_cell( base_world& world, visual_ptr v, bool beditor );
	virtual					~command_terrain_remove_cell	(  );
	virtual	void			execute				( );

private:
	base_world&						m_world;
	visual_ptr						m_visual;
	bool							m_beditor;

}; // class command_terrain_remove_cell

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_TERRAIN_REMOVE_CELL_H_INCLUDED