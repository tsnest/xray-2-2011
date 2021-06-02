////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/device_utils.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/physical_path_iterator.h>

namespace xray {
namespace fs_new {

signalling_bool   calculate_file_size	(synchronous_device_interface const &	device, 
										 file_size_type * const					out_file_size, 
										 native_path_string const &				physical_path,
										 assert_on_fail_bool					assert_on_fail)
{
	file_type * file					=	NULL;
	if ( !device->open(& file, physical_path, file_mode::open_existing, file_access::read, assert_on_fail) )
		return								false;
	
	device->seek							(file, 0, seek_file_end);
	* out_file_size						=	device->tell(file);
	device->close							(file);
	return									true;
}

bool   create_folder_r					(synchronous_device_interface const & device, 
										 native_path_string const & path, 
										 bool create_last)
{	
	native_path_string						absolute_path;
	if ( !convert_to_absolute_path(& absolute_path, path, assert_on_fail_true) )
		return								false;

	u32 const colon_pos					=	absolute_path.find(':');
	ASSERT									(colon_pos != u32(-1));
	u32 const drive_part_length			=	colon_pos + 2;
	native_path_string						cur_path;
	cur_path.assign							(absolute_path.c_str(), absolute_path.c_str() + drive_part_length);

	path_part_iterator	it					(absolute_path.c_str() + drive_part_length, 
											 int(absolute_path.length() - drive_part_length),
											 absolute_path.separator);

	path_part_iterator	end_it			=	path_part_iterator::end();
	ASSERT									(it != end_it);
	bool			result				=	true;
	while ( it != end_it )
	{
		native_path_string					part;
		it.append_to_string					(part);
		cur_path						+=	part;

		path_part_iterator next_it		=	it;
		++next_it;

		if ( !create_last && next_it == end_it )
			return							result;

		result							&=	device->create_folder(cur_path);
		cur_path						+=	native_path_string::separator;
		it								=	next_it;
	}

	return									result;
}

bool   create_folder_r					(synchronous_device_interface const &	device, 
										 pcstr									path, 
										 bool									create_last)
{
	return		create_folder_r				(device, fs_new::native_path_string::convert(path), create_last);
}

bool   erase_r							(synchronous_device_interface const &	device,
										 native_path_string const &				path)
{
	physical_path_info const & path_info	=	device->get_physical_path_info(path);
	if ( !path_info.exists() )
		return								true;

	for ( physical_path_iterator	it		=	path_info.children_begin(),
									it_end	=	path_info.children_end();
									it		!=	it_end;
											++it )
	{
		native_path_string	child_path;
		it.get_full_path					(& child_path);
		if ( !erase_r(device, child_path) )
			return							false;
	}								

	if ( !device->erase(path) )
		return								false;

	return									true;
}

void   calculate_file_hash				(synchronous_device_interface const &	device, 
										 file_type *							file,
										 u32 * const							out_file_hash)
{
	u32 const		chunk_size			=	128*1024;
	char			file_chunk				[chunk_size];

	* out_file_hash						=	0;
	do
	{
		u32 const bytes_read			=	(u32)device->read(file, file_chunk, chunk_size);
		if ( !bytes_read )
			break;

		* out_file_hash					=	crc32(file_chunk, bytes_read, * out_file_hash);

	} while ( xray::identity(true) ) ;
}

signalling_bool   calculate_file_hash	(synchronous_device_interface const &	device, 
										 native_path_string const &				file_path,
										 u32 * const							out_file_hash)
{
	file_type * file					=	NULL;
	if ( !device->open(& file, file_path, file_mode::open_existing, file_access::read, assert_on_fail_false) )
		return								false;
	
	calculate_file_hash						(device, file, out_file_hash);
	device->close							(file);
	return									true;
}

bool   files_equal						(synchronous_device_interface const & device, 
										 file_type * f1, file_type * f2, u32 size)
{
	u32 const	chunk_size				=	128 * 1024;
	char		file1_chunk					[chunk_size];
	char		file2_chunk					[chunk_size];
	u32			size_left				=	size;

	while ( size_left != 0 )
	{
		u32 const work_size				=	size_left < chunk_size ? size_left : chunk_size;
		u32 const bytes_read1			=	(u32)device->read(f1, file1_chunk, work_size);
		R_ASSERT_U							(bytes_read1 == work_size);
		u32 const bytes_read2			=	(u32)device->read(f2, file2_chunk, work_size);
		R_ASSERT_U							(bytes_read2 == work_size);
		
		if ( memcmp(file1_chunk, file2_chunk, work_size) )
			return							false;

		size_left						-=	work_size;
	}

	return									true;
}

// @post: position in files are changed
void   copy_data						(synchronous_device_interface const & device, 
										 file_type * f_dest, file_type * f_src, file_size_type size, 
										 u32 * hash, file_size_type size_to_hash)
{
	u32 const		chunk_size			=	128*1024;
	char			file_chunk				[chunk_size];
	file_size_type	size_left			=	size;

	if ( hash )
		*hash							=	0;

	file_size_type bytes_left_to_hash	=	(size_to_hash == u32(-1)) ? size : size_to_hash;
	while ( size_left != 0 )
	{
		u32 const work_size				=	(u32)(size_left < chunk_size ? size_left : chunk_size);
		u32 const bytes_read			=	(u32)device->read(f_src, file_chunk, work_size);
		if ( hash )
		{
			u32 const bytes_to_hash		=	(u32)(bytes_left_to_hash >= bytes_read ? bytes_read : bytes_left_to_hash);
			*hash						=	crc32(file_chunk, bytes_to_hash, *hash);
			bytes_left_to_hash			-=	bytes_to_hash;
		}

		R_ASSERT							(bytes_read == work_size);
		u32 const bytes_written			=	(u32)device->write(f_dest, file_chunk, work_size);
		XRAY_UNREFERENCED_PARAMETER			(bytes_written);
		R_ASSERT							(bytes_written == work_size);
		size_left						-=	work_size;
	}
}

} // namespace fs_new
} // namespace xray
