////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fat_header.h"
#include "nodes.h"

namespace xray {
namespace vfs {

void   fat_header::reverse_bytes ()
{ 
	vfs::reverse_bytes						(num_nodes); 
	vfs::reverse_bytes						(buffer_size); 
}

void   fat_header::set_big_endian () 
{ 
	strings::copy							(endian_string, big_endian_string); 
}

void   fat_header::set_little_endian () 
{ 
	strings::copy							(endian_string, little_endian_string);
}

bool   fat_header::is_big_endian () 
{ 
	R_ASSERT								(endian_string[0]); 
	return									strings::equal(endian_string, big_endian_string);
}

} // namespace vfs
} // namespace xray
