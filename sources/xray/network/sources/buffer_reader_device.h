////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_READER_DEVICE_H_INCLUDED
#define BUFFER_READER_DEVICE_H_INCLUDED

#include <xray/debug/signalling_bool.h>

namespace xray
{

class buffer_reader_device
{
public:
	typedef	u32							size_type;
	typedef	pbyte						pointer_type;
	typedef pcbyte						const_pointer_type;
	typedef xray::const_buffer			buffer_type;

	inline			buffer_reader_device	(const_pointer_type const buff,
											 size_type const buff_size);
	inline explicit	buffer_reader_device	(buffer_type const & buff);
	inline			~buffer_reader_device	();

	//core device interface
	inline	signalling_bool	read			(pointer_type ptr_dst,
											 size_type const dst_size);
	//seekable interface
	inline	signalling_bool	seek			(size_type const new_pos);
	inline	size_type		position		() const { return m_pos; };
	inline	size_type		size			() const { return m_buffer.size(); }

	//direct accessible interface
	inline	const_pointer_type	pointer			() const;
	inline	const_pointer_type	data			() const;
private:
	inline	size_type			capacity		() const;
	inline	const_pointer_type	current			() const;

	buffer_type			m_buffer;
	size_type			m_pos;
}; // class buffer_reader_device

} // namespace xray

#include "buffer_reader_device_inline.h"

#endif // #ifndef BUFFER_READER_DEVICE_H_INCLUDED