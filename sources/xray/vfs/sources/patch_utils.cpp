////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "patch.h"
#include <xray/compressor_ppmd.h>
#include <xray/memory_allocated_buffer.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/vfs/unpack.h>
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

using namespace		fs_new;

void   read_file_data					(allocated_buffer * const				out_buffer,
										 base_node<> * const					node,
										 synchronous_device_interface * const	device,
				 						 compressor * const						compressor,
										 memory::base_allocator * const			allocator)
{
	native_path_string const file_path	=	get_node_physical_path(node);

	file_type_pointer const file			(file_path, * device, file_mode::open_existing, file_access::read);
	R_ASSERT								(file, "cannot open file '%s'", file_path.c_str());
	if ( node->is_compressed() )
	{
		decompress_node						(node, out_buffer, * device, allocator, compressor);
	}
	else
	{
		u32 const file_size				=	get_file_size(node);
		out_buffer->assign					(file_size, allocator, "file_data");

		file_size_type const file_offs	=	get_file_offs(node);
		(* device)->seek					(file, file_offs, seek_file_begin);

		file_size_type const bytes_read	=	(* device)->read(file, out_buffer->c_ptr(), file_size);
		R_ASSERT_U							(bytes_read == file_size, "couldnot read %d bytes from file '%s'",
											 file_size, file_path.c_str());
	}
}

} // namespace vfs
} // namespace xray
