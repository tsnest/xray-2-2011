////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config_value.h>
#include <boost/crc.hpp>
#include <xray/render/core/destroy_data_helper.h>

namespace xray {
namespace render {

bool per_bytes_equal(u8 const* a_ptr, u8 const* b_ptr, u32 const num_comparision)
{
	for (u32 i=0; i<num_comparision; ++i, ++a_ptr, ++b_ptr)
		if( *a_ptr != *b_ptr)
			return false;
	return true;
}

bool custom_config_value::value_exists(pcstr key) const
{
	R_ASSERT					(type == xray::configs::t_table_named || type == xray::configs::t_table_indexed);
	const_iterator it			= begin();
	const_iterator it_e			= end();
	
	boost::crc_32_type			processor;
	processor.process_block		(key, key + strings::length(key));
	u32 const crc				= processor.checksum();
	
	const_iterator const result	= std::lower_bound(it, it_e, crc);
	if ( result == it_e)
		return					false;
	
	if ( result->id_crc != crc )
		return					false;
	
	return						strings::equal( key, result->id );
}

void custom_config_value::call_data_destructor() const
{
	if (destroyer)
	{
		if (count<=sizeof(u32))
			((destroy_data_helper_base*)&destroyer)->destroy(&data);
		else
			((destroy_data_helper_base*)&destroyer)->destroy(data);
	}
	
	if (type!=xray::configs::t_table_named && type!=xray::configs::t_table_indexed)
		return;
	
	for (const_iterator it=begin(); it!=end(); ++it)
	{
		it->call_data_destructor();
	}
}

custom_config_value const& custom_config_value::operator[](pcstr key) const
{
	R_ASSERT					(type == xray::configs::t_table_named);
	const_iterator it			= begin();
	const_iterator it_e			= end();
	
	boost::crc_32_type			processor;
	processor.process_block		(key, key + strings::length(key));
	u32 const crc				= processor.checksum();
	
	const_iterator const result	= std::lower_bound(it, it_e, crc);
	
	R_ASSERT					(result!=it_e, "item not found [%s]", key);
	R_ASSERT					(result->id_crc==crc, "item not found [%s]", id);
	
	do {
		if ( strings::equal( key, result->id ) )
			return				*result;
	} while ( result->id_crc == crc );
	
	UNREACHABLE_CODE			( return *result );
}

} // namespace render
} // namespace xray
