////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TERRAIN_CELL_REMOVE_TEXTURE_H_INCLUDED
#define COMMAND_TERRAIN_CELL_REMOVE_TEXTURE_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/visual.h>

namespace xray {
namespace render {

class base_world;

class command_terrain_cell_remove_texture :
	public engine::command,
	private boost::noncopyable
{
public:
							command_terrain_cell_remove_texture( base_world& world, visual_ptr v, u32 tex_user_id);
	virtual					~command_terrain_cell_remove_texture	(  );
	virtual	void			execute				( );

private:
	base_world&						m_world;
	visual_ptr						m_visual;
	u32								m_tex_user_id;

}; // class command_terrain_cell_remove_texture

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_TERRAIN_CELL_REMOVE_TEXTURE_H_INCLUDED