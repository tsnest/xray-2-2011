////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_VERTEX_BUFFER_H_INCLUDED
#define XRAY_RENDER_CORE_VERTEX_BUFFER_H_INCLUDED

#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

//enum {
//	LOCKFLAGS_FLUSH		= D3D_MAP_WRITE_DISCARD,
//	LOCKFLAGS_APPEND	= D3D_MAP_WRITE_NO_OVERWRITE
//};

class vertex_buffer {
public:
								vertex_buffer	( u32 size );
	inline	untyped_buffer&		buffer			( )	const;
	inline	u32					discard_id		( ) const;
	inline	u32					size			( ) const;

public:
	inline	void				flush			( );

	template < typename VertexType >
	inline	VertexType*			lock			( u32 vertex_count, u32& vertex_offset);

	template < typename VertexType >
	inline	void				lock			( u32 vertex_count, VertexType** buffer, u32& vertex_offset );
			pvoid				lock			( u32 vertex_count, u32 v_stride, u32& vertex_offset);
			void				unlock			( );

private:
	untyped_buffer_ptr			m_buffer;
	u32							m_size;			// size in bytes
	u32							m_position;		// position in bytes
	u32							m_discard_id;	// id of discard - usually for caching
	u32							m_lock_count;
	u32							m_lock_stride;

#ifdef DEBUG
public:
	u32							dbg_lock;
#endif
}; // class vertex_buffer

} // namespace render 
} // namespace xray

#include <xray/render/core/vertex_buffer_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_VERTEX_BUFFER_H_INCLUDED