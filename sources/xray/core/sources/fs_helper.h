////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_HELPER_H_INCLUDED
#define FS_HELPER_H_INCLUDED

namespace xray {
namespace fs {

template <class Ordinal>
void			reverse_bytes	(Ordinal& res);

// @post: position in files are changed
bool			files_equal		(file_type* f1, file_type* f2, u32 size);
// @post: position in files are changed
void			copy_data		(file_type* f_dest, file_type* f_src, file_size_type const size, u32* hash = NULL, file_size_type const size_to_hash = u32(-1));


u32				path_crc32		(pcstr const data, u32 const size, u32 const start_value = 0);
u32				crc32			(pcstr const data, u32 const size, u32 const start_value = 0);
u32				crc32			(u32 const starting_hash, u32 const hash_to_combine);

} // namespace fs
} // namespace xray 

#include "fs_helper_inline.h"

#endif // FS_HELPER_H_INCLUDED

