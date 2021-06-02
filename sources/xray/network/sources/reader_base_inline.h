////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef READER_BASE_INLINE_H_INCLUDED
#define READER_BASE_INLINE_H_INCLUDED

namespace xray
{

template <typename Reader, typename Device>
signalling_bool seekable_reader<Reader, Device>::advance( size_type const offset )
{
	Reader* this_ptr = static_cast<Reader*>(this);
	return this_ptr->m_device.seek(
		this_ptr->m_device.position() + offset) ? true : false;
		
}

template <typename Reader, typename Device>
signalling_bool seekable_reader<Reader, Device>::seek( size_type const new_position )
{
	Reader* this_ptr = static_cast<Reader*>(this);
	return this_ptr->m_device.seek(new_position) ? true : false;
}

template <typename Reader, typename Device>
signalling_bool seekable_reader<Reader, Device>::rewind( )
{
	return seek(0) ? true : false;
}

template <typename Reader, typename Device>
bool seekable_reader<Reader, Device>::eof( ) const
{
	Reader const * this_ptr = static_cast<Reader const *>(this);
	return this_ptr->m_device.position() == this_ptr->m_device.size();
}

template <typename Reader, typename Device>
typename seekable_reader<Reader, Device>::size_type
	seekable_reader<Reader, Device>::elapsed( ) const
{
	Reader const * this_ptr = static_cast<Reader const *>(this);
	return this_ptr->m_device.size() - this_ptr->m_device.position();
}

template <typename Reader, typename Device>
typename seekable_reader<Reader, Device>::size_type
	seekable_reader<Reader, Device>::tell( ) const
{
	Reader const * this_ptr = static_cast<Reader const *>(this);
	return this_ptr->m_device.position();	
}

template <typename Reader, typename Device>
typename seekable_reader<Reader, Device>::size_type
	seekable_reader<Reader, Device>::length( ) const
{
	Reader const * this_ptr = static_cast<Reader const *>(this);
	return this_ptr->m_device.size();
}

template <typename SeekableReader>
void strict_seekable_reader<SeekableReader>::advance( size_type const offset )
{
	bool result = SeekableReader::advance(offset);
	ASSERT(result);
	XRAY_UNREFERENCED_PARAMETER(result);
}

template <typename SeekableReader>
void strict_seekable_reader<SeekableReader>::seek( size_type const new_position )
{
	bool result = SeekableReader::seek(new_position);
	ASSERT(result);
	XRAY_UNREFERENCED_PARAMETER(result);
}

template <typename SeekableReader>
void strict_seekable_reader<SeekableReader>::rewind	( )
{
	bool result = SeekableReader::rewind();
	ASSERT(result);
	XRAY_UNREFERENCED_PARAMETER(result);
}

template <typename SeekableReader>
bool strict_seekable_reader<SeekableReader>::eof( ) const
{
	return SeekableReader::eof();
}

template <typename SeekableReader>
typename strict_seekable_reader<SeekableReader>::size_type
	strict_seekable_reader<SeekableReader>::elapsed( ) const
{
	return SeekableReader::elapsed();
}

template <typename SeekableReader>
typename strict_seekable_reader<SeekableReader>::size_type
	strict_seekable_reader<SeekableReader>::tell( ) const
{
	return SeekableReader::tell();
}

template <typename SeekableReader>
typename strict_seekable_reader<SeekableReader>::size_type
	strict_seekable_reader<SeekableReader>::length( ) const
{
	return SeekableReader::length();
}

template <typename Reader, typename Device>
typename direct_accessible_reader<Reader, Device>::const_pointer_type
	direct_accessible_reader<Reader, Device>::pointer() const
{
	Reader const * this_ptr = static_cast<Reader const *>(this);
	return this_ptr->m_device.pointer();
}

template <typename Reader, typename Device>
typename direct_accessible_reader<Reader, Device>::const_pointer_type
	direct_accessible_reader<Reader, Device>::data() const
{
	Reader* this_ptr = static_cast<Reader*>(this);
	return this_ptr->m_device.data();
}

} // namespace xray

#endif // #ifndef READER_BASE_INLINE_H_INCLUDED