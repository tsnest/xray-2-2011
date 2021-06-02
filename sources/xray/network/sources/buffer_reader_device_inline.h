////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_READER_DEVICE_INLINE_H_INCLUDED
#define BUFFER_READER_DEVICE_INLINE_H_INCLUDED

namespace xray
{

inline buffer_reader_device::buffer_reader_device(const_pointer_type const buff,
												  size_type const buff_size) :
	m_buffer	(buff, buff_size),
	m_pos		(0)
{
}

inline buffer_reader_device::buffer_reader_device(buffer_type const & buff) :
	m_buffer	(buff),
	m_pos		(0)
{
}

inline buffer_reader_device::~buffer_reader_device()
{
}

inline signalling_bool buffer_reader_device::read(pointer_type ptr_dst,
												  size_type const dst_size)
{
	if (dst_size > capacity())
		return false;

	xray::memory::copy	(ptr_dst, dst_size, current(), dst_size);
	m_pos				+= dst_size;
	return true;
}

inline signalling_bool	buffer_reader_device::seek(size_type const new_pos)
{
	if (new_pos > size())
		return false;
	
	m_pos = new_pos;
	return true;
}

inline buffer_reader_device::size_type		buffer_reader_device::capacity() const
{
	return size() - position();
}

inline buffer_reader_device::const_pointer_type		buffer_reader_device::current() const
{
	return data() + position();
}

inline	buffer_reader_device::const_pointer_type	buffer_reader_device::pointer() const
{
	return current();
}

inline	buffer_reader_device::const_pointer_type	buffer_reader_device::data() const
{
	return pointer_cast<const_pointer_type>(m_buffer.c_ptr());
}

} // namespace xray

#endif // #ifndef BUFFER_READER_DEVICE_INLINE_H_INCLUDED