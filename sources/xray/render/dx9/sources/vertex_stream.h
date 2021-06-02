////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef VERTEX_STREAM_H_INCLUDED
#define VERTEX_STREAM_H_INCLUDED

namespace xray {
namespace render {

enum
{
	LOCKFLAGS_FLUSH		= D3DLOCK_DISCARD,
	LOCKFLAGS_APPEND	= D3DLOCK_NOOVERWRITE
};

class vertex_stream
{
public:
	void						create();
	void						destroy();
	void						reset_begin();
	void						reset_end();

	ID3DVertexBuffer*	get_buffer() {return m_vb;}
	u32					get_discard_id() {return m_discard_id;}
	void				flush() {m_position=m_size;}

	void*	lock(u32 v_count, u32 v_stride, u32& v_offset);
	void	unlock(u32 v_count, u32 v_stride);
	u32		get_size() {return m_size;}

	vertex_stream();
	~vertex_stream() {destroy();};

public:
	ID3DVertexBuffer*	old_vb;
#ifdef DEBUG
	u32					m_dbg_lock;
#endif

private:
	void clear();

private :
	ID3DVertexBuffer*	m_vb;
	u32					m_size;			// size in bytes
	u32					m_position;		// position in bytes
	u32					m_discard_id;		// id of discard - usually for caching
}; // class vertex_stream

} // namespace render 
} // namespace xray



#endif // #ifndef VERTEX_STREAM_H_INCLUDED