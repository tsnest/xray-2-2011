////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_UNPACK_H_INCLUDED
#define VFS_UNPACK_H_INCLUDED

#include <xray/fs/native_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/compressor_ppmd.h>
#include <xray/memory_allocated_buffer.h>
#include <xray/vfs/mount_args.h>

namespace xray {
namespace vfs {

typedef	boost::function< void (u32 unpacked_count, u32 all_count, pcstr name, u32 flags) >
										unpack_callback;
typedef	boost::function< bool (base_node<> * node) >
										unpack_filter_callback;

struct unpack_arguments : public core::noncopyable
{
	unpack_arguments					(fs_new::virtual_path_string const &	source_path, 
										 fs_new::native_path_string	const &		target_path, 
										 fs_new::synchronous_device_interface &	device,
										 ppmd_compressor &						compressor,
										 memory::base_allocator *				allocator,
										 unpack_callback const &				callback,
										 logging::log_format *					log_format	=	NULL,
										 logging::log_flags_enum				log_flags	=	(logging::log_flags_enum)0,
										 unpack_filter_callback const &			filter_callback = NULL)
		:	source_path(source_path), target_path(target_path), callback(callback),
			nodes_count(0), unpacked_nodes_count(0), device(device), log_format(log_format), log_flags(log_flags),
			compressor(compressor), allocator(allocator), filter_callback(filter_callback) {;}

	memory::base_allocator *			allocator;
	ppmd_compressor &					compressor;
	fs_new::synchronous_device_interface &	device;
	fs_new::virtual_path_string			source_path;
	fs_new::native_path_string			target_path;
	unpack_callback						callback;
	unpack_filter_callback				filter_callback;
	logging::log_format *				log_format;
	logging::log_flags_enum				log_flags;
	u32									nodes_count;
	u32									unpacked_nodes_count;
};

class	virtual_file_system;

bool   unpack							(virtual_file_system &	file_system, unpack_arguments & args);

result_enum   decompress_node			(base_node<> *							node, 
										 allocated_buffer *						uncompressed_data, 
										 fs_new::synchronous_device_interface &	device,
										 memory::base_allocator *				allocator,
										 compressor *							compressor);


} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_UNPACK_H_INCLUDED