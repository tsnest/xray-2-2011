////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_CAMERA_H_INCLUDED
#define XRAY_RENDER_ENGINE_CAMERA_H_INCLUDED

namespace xray {
namespace render {

class camera {
public:
	inline					camera						( );

	inline void				set_view_transform			( float4x4 const & matrix);
#ifndef MASTER_GOLD
	inline void				set_view_transform_only		( float4x4 const & matrix);
#endif // #ifndef MASTER_GOLD
	inline void				set_projection_transform	( float4x4 const & matrix);

	inline float4x4 const&	get_view_transform			( ) const	{ return m_view;}
#ifndef MASTER_GOLD
	inline float4x4 const&	get_culling_view_transform	( ) const	{ return m_culling_view;}
#endif // #ifndef MASTER_GOLD
	inline float4x4 const&	get_view_inverted_transform	( ) const	{ return m_view_inverted;}
	inline float4x4 const&	get_projection_transform	( ) const	{ return m_projection;}

private:
	float4x4				m_view;
	float4x4				m_view_inverted;
	float4x4				m_projection;
#ifndef MASTER_GOLD
	float4x4				m_culling_view;
#endif // #ifndef MASTER_GOLD
}; // class camera

} // namespace render
} // namespace xray

#include "camera_inline.h"

#endif // #ifndef XRAY_RENDER_ENGINE_CAMERA_H_INCLUDED