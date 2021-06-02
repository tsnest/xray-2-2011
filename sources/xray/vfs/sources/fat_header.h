////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_FAT_HEADER_H_INCLUDED
#define VFS_FAT_HEADER_H_INCLUDED

namespace xray {
namespace vfs {

static const pcstr	big_endian_string		=	"big-endian";
static const pcstr	little_endian_string	=	"little-endian";

struct	fat_header
{
private:
	char	endian_string[14];
public:
	u32		num_nodes;
	u32		buffer_size;

			fat_header			() : num_nodes(0), buffer_size(0) { memory::zero(endian_string); }

	void 	set_big_endian		();
	void 	set_little_endian	();
	bool 	is_big_endian		();
	void 	reverse_bytes		();
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_FAT_HEADER_H_INCLUDED