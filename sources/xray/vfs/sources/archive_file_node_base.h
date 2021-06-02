////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_ARCHIVE_FILE_NODE_BASE_H_INCLUDED
#define VFS_ARCHIVE_FILE_NODE_BASE_H_INCLUDED

#include <xray/vfs/base_node.h>

namespace xray {
namespace vfs {

#pragma pack(push, 8)

template <platform_pointer_enum T = platform_pointer_default>
class archive_file_node_base
{
public:
	typedef		base_node<T>				node_type;
	typedef		typename node_type::file_size_type	file_size_type;

public:
										archive_file_node_base()
											: size_in_db(0), pos_in_db(0), hash(0) {}

	void			set_hash			(u32 hash) { this->hash = hash; }
	u32				get_hash			() const { return hash; }
	void			set_size			(u32 size) { size_in_db = size; }
	u32				get_size			() const { return size_in_db; }
	void			set_pos				(file_size_type pos) { pos_in_db = pos; }
	file_size_type	get_pos				() const { return pos_in_db; }

	void		reverse_bytes			()
	{
		vfs::reverse_bytes					(size_in_db);
		vfs::reverse_bytes					(pos_in_db);
		vfs::reverse_bytes					(hash);
	}

public:
	u32										size_in_db;
	file_size_type							pos_in_db;
	u32										hash;
};

#pragma pack(pop)

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_ARCHIVE_FILE_NODE_BASE_H_INCLUDED