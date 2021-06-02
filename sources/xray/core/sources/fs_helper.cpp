////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_helper.h"
#include <xray/fs_path.h>
#include <xray/fs_path_iterator.h>
#include <boost/crc.hpp>
#include <xray/fs_utils.h>

namespace xray {
namespace fs {

// @post: position in files are changed
bool   files_equal (file_type * f1, file_type * f2, u32 size)
{
	seek_file						(f1, 0, SEEK_SET);
	seek_file						(f2, 0, SEEK_SET);
	u32 const	chunk_size		=	128*1024;
	char		file1_chunk			[chunk_size];
	char		file2_chunk			[chunk_size];
	u32			size_left		=	size;

	while ( size_left != 0 )
	{
		u32 const work_size		=	size_left < chunk_size ? size_left : chunk_size;
		u32 const bytes_read1	=	(u32)read_file(f1, file1_chunk, work_size);
		R_ASSERT_U					(bytes_read1 == work_size);
		u32 const bytes_read2	=	(u32)read_file(f2, file2_chunk, work_size);
		R_ASSERT_U					(bytes_read2 == work_size);
		
		if ( memcmp(file1_chunk, file2_chunk, work_size) )
		{
			return false;
		}

		size_left				-=	work_size;
	}

	return							true;
}

// @post: position in files are changed
void   copy_data (file_type* f_dest, file_type* f_src, file_size_type size, u32* hash, file_size_type size_to_hash)
{
	u32 const		chunk_size	=	128*1024;
	char			file_chunk		[chunk_size];
	file_size_type	size_left	=	size;

	if ( hash )
		*hash					=	0;

	file_size_type bytes_left_to_hash	=	(size_to_hash == u32(-1)) ? size : size_to_hash;
	while ( size_left != 0 )
	{
		u32 const work_size		=	(u32)(size_left < chunk_size ? size_left : chunk_size);
		u32 const bytes_read	=	(u32)read_file(f_src, file_chunk, work_size);
		if ( hash )
		{
			u32 const bytes_to_hash	=	(u32)(bytes_left_to_hash >= bytes_read ? bytes_read : bytes_left_to_hash);
			*hash					=	crc32(file_chunk, bytes_to_hash, *hash);
			bytes_left_to_hash		-=	bytes_to_hash;
		}

		R_ASSERT					(bytes_read == work_size);
		u32 const bytes_written	=	(u32)write_file(f_dest, file_chunk, work_size);
		XRAY_UNREFERENCED_PARAMETER	(bytes_written);
		R_ASSERT					(bytes_written == work_size);
		size_left				-=	work_size;
	}
}

typedef	boost::crc_optimal<32, 0x04C11DB7, 0, 0, true, false>	crc_processor;

u32   crc32 (pcstr const data, u32 const size, u32 const start_value)
{
	crc_processor	processor	(start_value);
	processor.process_bytes		(data, size);
	return processor.checksum() & u32(-1);
}

u32	  crc32 (u32 const starting_hash, u32 const hash_to_combine)
{
	return									starting_hash ^ hash_to_combine;
}

u32   path_crc32 (pcstr const data, u32 const size, u32 const start_value)
{
	crc_processor	processor	(start_value);

	int start_index						=	-1;
	int cur_index						=	0;
	while ( cur_index < (int)size )
	{
		const bool is_path_separator	=	(data[cur_index] == '/') || 
											(data[cur_index] == '\\');
		if ( is_path_separator )
		{
			if ( start_index != -1 )
			{
				processor.process_block		(data+start_index, data+cur_index);
				start_index				=	-1;
			}
		}
		else if ( start_index == -1 )
		{
			start_index					=	cur_index;
		}
		
		++cur_index;
	}
	
	if ( start_index != -1 )
	{
		processor.process_block				(data+start_index, data+cur_index);
	}

	return processor.checksum				();
}

} // namespace fs
} // namespace xray