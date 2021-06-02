////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_TEXTURE_POOL_H_INCLUDED
#define TERRAIN_TEXTURE_POOL_H_INCLUDED

#include <xray/render/facade/common_types.h>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <xray/render/core/res_texture.h>

namespace xray {
namespace render {

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

struct terrain_texture_item
{
	terrain_texture_item(): loaded ( false) {}

	fs_new::virtual_path_string	name;
	bool				loaded;
};


class terrain_texture_pool : public boost::noncopyable 
{
public:

	terrain_texture_pool	( u32 const size, u32 const textures_count);
	~terrain_texture_pool	();

	int		add_texture			( fs_new::virtual_path_string const& name, bool deferred_load = false);
	void	remove_texture		( u32 id);
	bool	exchange_texture	( fs_new::virtual_path_string const& old_texture, fs_new::virtual_path_string const& new_texture, bool deffered_load = false);
	int		get_texture_id		( fs_new::virtual_path_string const& name);

	void						load_textures	();
	math::rectangle<math::int2>	get_tile_rect	( u32 const ind);

	//char const*					get_name()		{ return m_texture_name.c_str(); }

	ref_texture					texture_diffuse()		{ return m_pool_texture_diffuse; }
	ref_texture					texture_nmap()			{ return m_pool_texture_nmap; }
	ref_texture					texture_spec()			{ return m_pool_texture_spec; }
	

private:

	static u32				s_new_pool_texture_id;

inline	void				load_texture	( u32 ind);

	math::uint2				m_tiles;
	u32						m_tile_size;
	ref_texture				m_pool_texture_diffuse;
	ref_texture				m_pool_texture_nmap;
	ref_texture				m_pool_texture_spec;
	const u32				m_textures_count;
	boost::array<terrain_texture_item, render::terrain_texture_max_count> m_texture_items;
}; // class terrain_texture_pool

} // namespace render
} // namespace xray

#endif // TERRAIN_TEXTURE_POOL_H_INCLUDED
