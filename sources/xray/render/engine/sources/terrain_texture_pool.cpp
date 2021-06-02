////////////////////////////////////////////////////////////////////////////
//	Created		: 05.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_texture_pool.h"
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

u32		terrain_texture_pool::s_new_pool_texture_id = 0;

terrain_texture_pool::terrain_texture_pool	( u32 const size, u32 const textures_count):
m_tiles				( (u32)math::ceil(math::sqrt( (float)textures_count)), (u32)math::ceil(math::sqrt( (float)textures_count))),
m_tile_size			( size),
m_textures_count	( m_tiles.x*m_tiles.y)
{
	//ASSERT( m_tiles.x*size <= 4096 && m_tiles.y*size <= 4096);

	fixed_string<32>		texture_name;

	texture_name.assignf( "$terrain_diffuse%d$", s_new_pool_texture_id);
	m_pool_texture_diffuse		= resource_manager::ref().create_texture2d( texture_name.c_str(), size, size, NULL, DXGI_FORMAT_BC1_UNORM_SRGB /*D3DFMT_DXT1 not sure*/, D3D_USAGE_DEFAULT, 9/*9*/, 16);
	ASSERT( m_pool_texture_diffuse);

	texture_name.assignf("$terrain_nmap%d$", s_new_pool_texture_id);
	m_pool_texture_nmap	= resource_manager::ref().create_texture2d( texture_name.c_str(), size, size, NULL, DXGI_FORMAT_BC3_UNORM /*D3DFMT_DXT1 not sure*/, D3D_USAGE_DEFAULT, 9/*9*/, 16);
	ASSERT( m_pool_texture_nmap);

	texture_name.assignf("$terrain_spec%d$", s_new_pool_texture_id);
	m_pool_texture_spec	= resource_manager::ref().create_texture2d( texture_name.c_str(), size, size, NULL, DXGI_FORMAT_BC1_UNORM_SRGB /*D3DFMT_DXT1 not sure*/, D3D_USAGE_DEFAULT, 9/*9*/, 16);
	ASSERT( m_pool_texture_spec);

	++s_new_pool_texture_id;
}

terrain_texture_pool::~terrain_texture_pool	()
{

}

int	terrain_texture_pool::add_texture		( fs_new::virtual_path_string const & name, bool deferred_load)
{
	int		ind		= -1;
	for( u32 i = 0; i < m_textures_count; ++i)
	{
		if( m_texture_items[i].name.length() == 0 )
		{
			if( ind == -1)
			{
				ind = i;
				break;
			}
		}
		else if( m_texture_items[i].name == name)
		{
			if( !deferred_load && !m_texture_items[i].loaded)
			{
				load_texture(i);
				m_texture_items[i].loaded = true;
			}

			return i;
		}
	}

	if( ind >= 0)
	{
		m_texture_items[ind].name		= name;
		if( !deferred_load)
		{
			load_texture(ind);
			m_texture_items[ind].loaded		= true;
		}
		else
			m_texture_items[ind].loaded		= false;
	}

	return ind;
}

void terrain_texture_pool::remove_texture	( u32 id)
{
	ASSERT( id < render::terrain_texture_max_count);
	ASSERT( m_texture_items[id].name != "" );

	m_texture_items[id].name = "";
	m_texture_items[id].loaded	= false;
}

void terrain_texture_pool::load_textures	()
{
	for( u32 i = 0; i < m_textures_count; ++i)
	{
		if( m_texture_items[i].name.length() > 0 && !m_texture_items[i].loaded )
		{
			load_texture(i);
			m_texture_items[i].loaded = true;
		}
	}
}

math::rectangle<math::int2>	terrain_texture_pool::get_tile_rect	(u32 ind)
{
	ASSERT(ind < m_tiles.x*m_tiles.y);
	int top		= ind/m_tiles.x;
	int left	= ind-top*m_tiles.x;

	return math::rectangle<math::int2>( math::int2(left*m_tile_size, top*m_tile_size), math::int2((left+1)*m_tile_size, (top+1)*m_tile_size));
}

bool	terrain_texture_pool::exchange_texture( fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const &  new_texture, bool deffered_load)
{
	int	ind = get_texture_id( old_texture);
	ASSERT( ind >= 0);

	m_texture_items[ind].name		= new_texture;

	if( !deffered_load)
	{
		m_texture_items[ind].loaded		= false;
		load_texture(ind);
		m_texture_items[ind].loaded		= true;	
	}
	else
		m_texture_items[ind].loaded		= false;
	
	return false;
}

int		terrain_texture_pool::get_texture_id	( fs_new::virtual_path_string const & name)
{
	for( u32 i = 0; i < m_textures_count; ++i)
	{
		if( m_texture_items[i].name ==  name)
			return i;
	}
	return -1;
}

void	terrain_texture_pool::load_texture	( u32 ind)
{
	ASSERT( m_texture_items[ind].name.length() > 0 && !m_texture_items[ind].loaded);
	resource_manager::ref().copy_texture_from_file( &*m_pool_texture_diffuse, get_tile_rect(ind), ind, m_texture_items[ind].name.c_str() );	

	fs_new::virtual_path_string	nmap_texture_name;
	nmap_texture_name.assignf( "%s_nmap", m_texture_items[ind].name.c_str());
	resource_manager::ref().copy_texture_from_file( &*m_pool_texture_nmap, get_tile_rect(ind), ind,  nmap_texture_name.c_str());

	fs_new::virtual_path_string	spec_texture_name;
	spec_texture_name.assignf( "%s_spec", m_texture_items[ind].name.c_str());
	resource_manager::ref().copy_texture_from_file( &*m_pool_texture_spec, get_tile_rect(ind), ind,  spec_texture_name.c_str());
}

} // namespace render
} // namespace xray
