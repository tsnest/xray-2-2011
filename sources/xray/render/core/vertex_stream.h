////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VERTEX_STREAM_H_INCLUDED
#define VERTEX_STREAM_H_INCLUDED

#include "res_buffer.h"
namespace xray {
namespace render_dx10 {

enum
{
	LOCKFLAGS_FLUSH		= D3D_MAP_WRITE_DISCARD,
	LOCKFLAGS_APPEND	= D3D_MAP_WRITE_NO_OVERWRITE
};

class vertex_stream
{
public:
	vertex_stream	();
	~vertex_stream	();

	void			create( u32 size);

// 	void						reset_begin	();
// 	void						reset_end	();

	res_buffer *				buffer			() 	{return &*m_vb;}
	ID3DVertexBuffer *			hw_buffer		() 	{return m_vb->hw_buffer();}
	u32							discard_id		() 	{return m_discard_id;}
	void						flush			() 	{m_position=m_size;}

	template<typename V>
	V *		lock		( u32 v_count, u32& v_offset)					{ return (V*) lock( v_count, sizeof(V), v_offset); }

	template<typename V>
	void	lock		( u32 v_count, V** buffer, u32& v_offset)		{ *buffer = (V*)lock( v_count, sizeof(V), v_offset); }

	void *	lock		( u32 v_count, u32 v_stride, u32& v_offset);

	void	unlock		();
	u32		get_size	()									{return m_size;}


// public:
// 	ref_buffer*			old_vb;

#ifdef DEBUG
	u32					m_dbg_lock;
#endif

// private:
// 	void clear();

private :
	ref_buffer			m_vb;
	u32					m_size;			// size in bytes
	u32					m_position;		// position in bytes
	u32					m_discard_id;	// id of discard - usually for caching

	u32					m_lock_count;
	u32					m_lock_stride;
}; // class vertex_stream

} // namespace render 
} // namespace xray



#endif // #ifndef VERTEX_STREAM_H_INCLUDED