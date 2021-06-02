////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/path_string_utils.h>
#include <xray/fs/portable_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <boost/crc.hpp>

namespace xray {
namespace fs_new {

typedef	boost::crc_optimal<32, 0x04C11DB7, 0, 0, true, false>	crc_processor;

u32   crc32								(pcstr const data, u32 const size, u32 const start_value)
{
	crc_processor	processor	(start_value);
	processor.process_bytes		(data, size);
	return processor.checksum() & u32(-1);
}

u32	  crc32								(u32 const starting_hash, u32 const hash_to_combine)
{
	return									starting_hash ^ hash_to_combine;
}

u32   path_crc32						(pcstr const data, u32 const size, u32 const start_value)
{
	crc_processor	processor				(start_value);

	int start_index						=	-1;
	int cur_index						=	0;
	while ( cur_index < (int)size )
	{
		const bool is_path_separator	=	(data[cur_index] == virtual_path_string::separator) || 
											(data[cur_index] == native_path_string::separator);
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
		processor.process_block				(data+start_index, data+cur_index);

	return processor.checksum				();
}

} // namespace fs_new
} // namespace xray