////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WRITER_BASE_INLINE_H_INCLUDED
#define WRITER_BASE_INLINE_H_INCLUDED

namespace xray
{

template <typename Writer, typename Device>
void seekable_writer<Writer, Device>::seek(size_type const new_pos)				
{
	Writer* this_ptr = static_cast<Writer*>(this);
	this_ptr->m_device.seek(new_pos);
}

template <typename Writer, typename Device>
typename seekable_writer<Writer, Device>::size_type const
	seekable_writer<Writer, Device>::position() const
{
	Writer const * this_ptr = static_cast<Writer const *>(this);
	return this_ptr->m_device.position();
}

} // namespace xray

#endif // #ifndef WRITER_BASE_INLINE_H_INCLUDED