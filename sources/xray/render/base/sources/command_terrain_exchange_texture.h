////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TERRAIN_EXCHANGE_TEXTURE_H_INCLUDED
#define COMMAND_TERRAIN_EXCHANGE_TEXTURE_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/render/base/common_types.h>

namespace xray {
namespace render {

class base_world;

class command_terrain_exchange_texture :
	public engine::command,
	private boost::noncopyable
{
public:
							command_terrain_exchange_texture( base_world& world, texture_string const& old_texture, texture_string const& new_texture);
	virtual	void			execute				( );

private:
	base_world&						m_world;
	texture_string					m_old;
	texture_string					m_new;

}; // class command_terrain_exchange_texture

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_TERRAIN_EXCHANGE_TEXTURE_H_INCLUDED