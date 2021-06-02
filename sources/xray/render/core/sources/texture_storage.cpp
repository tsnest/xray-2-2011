////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_storage.h"
#include <xray/render/core/utils.h>
#include "texture_pool.h"
#include <xray/render/core/res_texture.h>

namespace xray {
namespace render {

struct storage_pool_tmp_info
{
	u32 width;
	u32 height;
	u32 format;
	u32 mip_count;
	u32 count;
	u32 mem_usage;
};

u32 get_hash( u32 width, u32 height, DXGI_FORMAT format)
{
	return (utils::log_2( width) | (utils::log_2( height)<<5) | ( (u32)format<<10));
}

texture_storage::texture_storage():
m_initialized	(false)
{
	
}

texture_storage::~texture_storage() 
{
	pools::iterator			it	= m_pools.begin();
	pools::const_iterator	end	= m_pools.end();

	for( ; it != end; ++it)
		DELETE( it->second);
}

#if 0
void texture_storage::initialize( xray::configs::lua_config_value  const & initial_params, size_t memory_amount, D3D_USAGE usage)
{
	ASSERT( !m_initialized);

	if( m_initialized)
		return;

	m_initialized = true;

	vector<storage_pool_tmp_info> pool_infos;
	pool_infos.reserve( initial_params.size());

	xray::configs::lua_config_value::const_iterator it = initial_params.begin();
	xray::configs::lua_config_value::const_iterator end = initial_params.end();

	storage_pool_tmp_info tmp_info;

	float base_memory_usage = 0;
	for( ; it != end; ++it)
	{
		
		tmp_info.width	= (*it)["width"];
		tmp_info.height	= (*it)["height"];
		tmp_info.format	= (*it)["format"];
		tmp_info.count	= (*it)["count"];

		tmp_info.mip_count = utils::calc_mipmap_count( tmp_info.width, tmp_info.height);

		tmp_info.mem_usage = tmp_info.count*utils::calc_texture_size( tmp_info.width, tmp_info.height, (DXGI_FORMAT)tmp_info.format, tmp_info.mip_count);
		base_memory_usage += tmp_info.mem_usage;
		
		pool_infos.push_back(tmp_info);
	}

	float memory_mag_factor = memory_amount/base_memory_usage;

	u32 const infos_size = pool_infos.size();
	for( u32 i = 0; i < infos_size; ++i)
	{
		storage_pool_tmp_info const & curr_info = pool_infos[i];
		m_pools.insert( utils::mk_pair( get_hash( curr_info.width, curr_info.height, (DXGI_FORMAT)curr_info.format), 
										NEW(texture_pool)( curr_info.width, curr_info.height, (DXGI_FORMAT)curr_info.format, curr_info.mip_count, u32(curr_info.count*memory_mag_factor + 0.999f) /*ceil*/, usage )));
	}
}
#endif

res_texture*	texture_storage::get		( u32 width, u32 height, DXGI_FORMAT format)
{
	pools::iterator it = m_pools.find( get_hash( width, height, format));

	if( it == m_pools.end())
		return NULL;

	return it->second->get( );
}

void	texture_storage::release	( res_texture const* texture)
{
	pools::iterator it = m_pools.find( get_hash( texture->width(), texture->height(), texture->format()));

	if( it == m_pools.end())
	{
		ASSERT( false, "There is no initialized pool with specified parameters.");
		return;
	}
	
	it->second->release( texture);
}


} // namespace render
} // namespace xray
