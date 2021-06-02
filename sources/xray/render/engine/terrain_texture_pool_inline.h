////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_TEXTURE_POOL_INLINE_H_INCLUDED
#define TERRAIN_TEXTURE_POOL_INLINE_H_INCLUDED

namespace xray {
namespace render_dx10 {

u8	terrain_texture_pool::pack_tex_id		( u32 ind)
{
	ASSERT( ind < m_tiles.x*m_tiles.y);
	u8 top		= u8(ind/m_tiles.x);
	u8 left		= u8(ind-top*m_tiles.x);

	return u8(((left&0x0F)<<4)|top&0x0F);
}


} // namespace render
} // namespace xray

#endif // #ifndef TERRAIN_TEXTURE_POOL_INLINE_H_INCLUDED