////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_TEXTURE_POOL_H_INCLUDED
#define TERRAIN_TEXTURE_POOL_H_INCLUDED

#include <xray/render/base/common_types.h>

namespace xray {
namespace render {

struct terrain_texture_item
{
	texture_string	name;
	bool			loaded;
};


class terrain_texture_pool : public boost::noncopyable 
{
public:

	terrain_texture_pool	( u32 const size, u32 const textures_count);
	~terrain_texture_pool	();

	int		add_texture			( texture_string const& name, bool deferred_load = false);
	void	remove_texture		( u32 id);
	bool	exchange_texture	( texture_string const& old_texture, texture_string const& new_texture, bool deffered_load = false);
	int		get_texture_id		( texture_string const& name);

	void					load_textures	();
	math::rectangle<math::int2>	get_tile_rect	( u32 const ind);

	char const*				get_pool_texture_name() { return "$terrain_texture_pool$";}

private:

inline	void				load_texture	( u32 ind);

	math::uint2					m_tiles;
	u32						m_tile_size;
	ref_texture				m_pool_texture;
	const u32				m_textures_count;
	terrain_texture_item	m_texture_items[terrain_texture_max_count];

}; // class terrain_texture_pool

} // namespace render
} // namespace xray

#endif // TERRAIN_TEXTURE_POOL_H_INCLUDED
