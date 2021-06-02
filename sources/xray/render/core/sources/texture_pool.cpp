////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_pool.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/utils.h>

namespace xray {
namespace render {

texture_pool::texture_pool	( u32 width, u32 height, DXGI_FORMAT format, u32 mips, u32 count, D3D_USAGE usage):
m_width				(width),
m_height			(height),
m_format			(format),
m_mips				(mips),
m_memory_usage		(0),
m_unoccupied_count	(count)
{
	ASSERT( count > 0 );

	m_textures.reserve( count);
	for( u32 i = 0; i < count; ++i)	
		m_textures.push_back( slot( resource_manager::ref().create_texture2d_impl( m_width, m_height, NULL, m_format, usage, m_mips)));

	m_memory_usage = u32(0.3f*m_width*m_height*utils::get_format_4x4_pixel_size(m_format)/16);
}

texture_pool::~texture_pool	()
{
	for( u32 i = 0; i < m_textures.size(); ++i)	
		resource_manager::ref().release_impl( m_textures[i].texture);
}

res_texture* texture_pool::get	()
{
	if( m_unoccupied_count <= 0)
		return NULL;

	--m_unoccupied_count;

	slot slot = m_textures[m_unoccupied_count];

	slot.occupied = true;

	return slot.texture;
}

void texture_pool::release	( res_texture const* texture)
{
	const u32 count = m_textures.size();
	
	for( u32 i = 0; i < count; ++i)
	{
		if( m_textures[i].texture != texture)
			continue;

		m_textures[i].occupied = false;
		std::swap( *(m_textures.begin() + i), *(m_textures.begin() + m_unoccupied_count));
		return;
	}

	ASSERT( false, "The texture could not be found in the pool!");
}

} // namespace render
} // namespace xray

