////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_VERTEX_BUFFER_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_VERTEX_BUFFER_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline untyped_buffer& vertex_buffer::buffer( )	const
{
	return				*m_buffer;
}

inline u32 vertex_buffer::discard_id		( ) const
{
	return				m_discard_id;
}

inline u32 vertex_buffer::size				( ) const
{
	return				m_size;
}

template < typename VertexType >
inline VertexType* vertex_buffer::lock		( u32 const vertex_count, u32& vertex_offset)
{
	pvoid const result	= lock( vertex_count, sizeof(VertexType), vertex_offset);
	return				static_cast< VertexType* >( result );
}

template < typename VertexType >
inline void vertex_buffer::lock				( u32 const vertex_count, VertexType** const buffer, u32& vertex_offset )
{
	*buffer				= lock<VertexType>( vertex_count, vertex_offset );
}

inline void vertex_buffer::flush			( )
{
	m_position			= m_size;
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_VERTEX_BUFFER_INLINE_H_INCLUDED