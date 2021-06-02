////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_ARCHIVE_INLINE_FILE_NODE_BASE_H_INCLUDED
#define VFS_ARCHIVE_INLINE_FILE_NODE_BASE_H_INCLUDED

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T>
class archive_inline_file_node_base
{
public:
										archive_inline_file_node_base () : m_inlined_data(0), m_inlined_size(0) {}

	void								set_inlined_data(const_buffer const & buffer) 
	{ 
		m_inlined_data					=	(pcstr)buffer.c_ptr	(); 
		m_inlined_size					=	(u32)buffer.size	(); 
	}
	const_buffer						get_inlined_data	() const { return const_buffer((pcstr)m_inlined_data, m_inlined_size); }
	void								reverse_bytes		()
	{
		vfs::reverse_bytes					(m_inlined_data);
		vfs::reverse_bytes					(m_inlined_size);
	}

public:
	typename platform_pointer<char const, T>::type	m_inlined_data;
	u32												m_inlined_size;

}; // class archive_inline_file_base

#pragma pack(pop)

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_ARCHIVE_INLINE_FILE_NODE_BASE_H_INCLUDED