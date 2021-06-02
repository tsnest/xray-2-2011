////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_DEVICE_INLINE_H_INCLUDED
#define BUFFER_DEVICE_INLINE_H_INCLUDED

namespace xray
{

inline buffer_writer_device::buffer_writer_device(pointer_type buff,
												  size_type const & buff_size) :
	m_buffer	(buff, buff_size),
	m_pos		(0)
{
}

inline buffer_writer_device::buffer_writer_device(buffer_type & buf)
{
	m_buffer	= buf;
	m_pos		= 0;
}

inline buffer_writer_device::~buffer_writer_device()
{
}

inline buffer_writer_device::size_type		buffer_writer_device::capacity() const
{
	return m_buffer.size() - m_pos;
}

inline buffer_writer_device::pointer_type	buffer_writer_device::current()
{
	typedef buffer_writer_device::pointer_type pointer_type;
	return static_cast<pointer_type>(m_buffer.c_ptr()) + m_pos;
}

inline void buffer_writer_device::write(const_pointer_type const ptr,
										size_type const size)
{
	R_ASSERT(size <= capacity());
	xray::memory::copy(current(), capacity(), ptr, size);
	m_pos += size;
}

inline	void buffer_writer_device::seek(size_type const new_pos)
{
	R_ASSERT((new_pos >= 0) && (new_pos <= m_buffer.size()));
	m_pos = new_pos;
}

} // namespace xray

#endif // #ifndef BUFFER_DEVICE_INLINE_H_INCLUDED