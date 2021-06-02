////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_WORLD_XBOX_360_INLINE_H_INCLUDED
#define XRAY_RENDER_ENGINE_WORLD_XBOX_360_INLINE_H_INCLUDED

namespace xray {
namespace render {
namespace engine {

inline pcstr world::type		( )
{
	return	( "xbox360" );
}

inline D3DDevice& world::device	( ) const
{
	ASSERT	( m_device );
	return	( *m_device );
}

inline u32 world::frame_id		( )
{
	return	( m_frame_id );
}

} // namespace engine
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_WORLD_XBOX_360_INLINE_H_INCLUDED