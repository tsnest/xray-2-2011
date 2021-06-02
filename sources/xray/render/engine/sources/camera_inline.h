////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_CAMERA_INLINE_H_INCLUDED
#define XRAY_RENDER_ENGINE_CAMERA_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline camera::camera							( )
{
	m_view.identity			( );
	m_view_inverted.identity( );
#ifndef MASTER_GOLD
	m_culling_view.identity	( );
#endif // #ifndef MASTER_GOLD
	m_projection.identity	( );
}

inline void camera::set_view_transform			( float4x4 const & matrix)
{
	m_view					= matrix;
	m_view_inverted			= invert4x3( m_view );
#ifndef MASTER_GOLD
	m_culling_view			= matrix;
#endif // #ifndef MASTER_GOLD
}

#ifndef MASTER_GOLD
inline void camera::set_view_transform_only		( float4x4 const & matrix)
{
	m_view					= matrix;
	m_view_inverted			= invert4x3( m_view );
}
#endif // #ifndef MASTER_GOLD

inline void camera::set_projection_transform	( float4x4 const & matrix)
{
	m_projection			= matrix;
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_CAMERA_INLINE_H_INCLUDED