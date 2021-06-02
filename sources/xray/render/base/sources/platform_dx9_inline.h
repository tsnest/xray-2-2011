////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef WORLD_INLINE_H_INCLUDED
#define WORLD_INLINE_H_INCLUDED

namespace xray {
namespace render {
namespace dx9 {


inline pcstr platform::type				( )
{
	return	( "dx9" );
}

inline u32 platform::frame_id				( )
{
	return	( m_render_frame_id );
}

inline IDirect3DDevice9& platform::device	( ) const
{
	//ASSERT	( m_device );
	return	*hw_wrapper::get_ref().device();//( *m_device );
}

} // namespace dx9
} // namespace render 
} // namespace xray 


#endif // #ifndef WORLD_INLINE_H_INCLUDED