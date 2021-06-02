////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INDEX_STREAM_H_INCLUDED
#define INDEX_STREAM_H_INCLUDED

namespace xray {
namespace render {

class index_stream
{
public:
	void	create();
	void	destroy();
	void	reset_begin();
	void	reset_end();

	ID3DIndexBuffer*	get_buffer() {return m_ib;}

	u32		discardid() {return m_discard_id;}
	void	flush() {m_position=m_size;}
	u16*	lock(u32 i_count, u32& i_offset);
	void	unlock(u32 i_count);

	index_stream() {clear();};
	~index_stream() {destroy();};

public:
	ID3DIndexBuffer*	old_ib;

private:
	void clear()
	{
		m_ib = NULL;
		m_size = 0;
		m_position = 0;
		m_discard_id = 0;
	}

private :
	ID3DIndexBuffer*	m_ib;
	u32					m_size;		// real size (usually mcount, aligned on 512b boundary)
	u32					m_position;
	u32					m_discard_id;
}; // class index_stream

} // namespace render 
} // namespace xray 



#endif // #ifndef INDEX_STREAM_H_INCLUDED