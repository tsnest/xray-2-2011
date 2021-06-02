////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/destroy_data_helper.h>
#include <xray/render/core/custom_config.h>

namespace xray {
namespace render {

effect_options_descriptor::effect_options_descriptor():
	id		(NULL),
	data	(NULL),
	bytes	(0),
	type	(0),
	count	(0),
	destroyer(0),
	memory_size(0)
{}

effect_options_descriptor::effect_options_descriptor(pcvoid in_data, u16 in_size)
{
	data	= (pbyte)in_data + sizeof(effect_options_descriptor);
	type	= configs::t_table_named;
	bytes	= 0;
	count	= 0;
	id		= 0;
	destroyer = 0;
	memory_size = in_size;
}

effect_options_descriptor::~effect_options_descriptor()
{
	// Created effect configs call destructors.
	//call_destructors();
}

void effect_options_descriptor::call_destructors()
{
	if (destroyer && data)
	{
		if (count<=sizeof(u32))
			((destroy_data_helper_base*)&destroyer)->destroy(&data);
		else
			((destroy_data_helper_base*)&destroyer)->destroy(data);
	}
	
	if (type==configs::t_table_named)
	{
 		effect_options_descriptor* children = (effect_options_descriptor*)data;
		R_ASSERT	( children );
 		for (u32 i=0; i<count; i++)
 		{
 			children->call_destructors();
 			children = (effect_options_descriptor*)((byte*)children + children->get_num_used_bytes());
 		}
	}
}

u32 effect_options_descriptor::get_crc () const
{
	u32 num_bytes_for_data = calc_config_memory_usage(*this);
	
	pcvoid mem_buffer = ALLOCA(num_bytes_for_data);
	
	xray::mutable_buffer mbuffer( pbyte(mem_buffer), num_bytes_for_data );
	
	u32 crc;
	custom_config_ptr config = create_custom_config(*this, mbuffer, crc, true);
	config->own_buffer		 = false;
	config->call_destructors = false;

	return crc; 
}

u32 effect_options_descriptor::get_data_memory_usage(u32& need_bytes_to_align4, u32& last_align_value) const
{
	u32 result = 0;
	
	if (id)
		result = strings::length(id) + 1;

	if (type==configs::t_table_named)
	{
		effect_options_descriptor* children = (effect_options_descriptor*)data;
		for (u32 i=0; i<count; i++)
		{
			result += children->get_data_memory_usage(need_bytes_to_align4, last_align_value);
			children = (effect_options_descriptor*)((byte*)children + children->get_num_used_bytes());
		}
	}
	else
	{
		if (count > sizeof(u32))
			result += count;
		
		u32 const rem = result % 4;
		
		if (rem)
		{
			last_align_value = 4 - rem;
			need_bytes_to_align4 += last_align_value;
		}
	}
	return result;
}

u32 effect_options_descriptor::get_num_total_fields() const
{
	if (type!=configs::t_table_named)
		return 0;
	
	u32 result = count;
	
	effect_options_descriptor* children = (effect_options_descriptor*)data;
	for (u32 i=0; i<count; i++)
	{
		result += children->get_num_total_fields();
		children = (effect_options_descriptor*)((byte*)children + children->get_num_used_bytes());
	}
	return result;
}

u32 effect_options_descriptor::get_num_used_bytes() const
{
	u32 result = bytes;
	
	if (type==configs::t_table_named)
	{
		effect_options_descriptor* children = (effect_options_descriptor*)data;
		for (u32 i=0; i<count; i++)
		{
			result += children->get_num_used_bytes();
			children = (effect_options_descriptor*)((byte*)children + children->get_num_used_bytes());
		}
	}
	return result;
}

effect_options_descriptor& effect_options_descriptor::operator[](pcstr key)
{
	u32 offset = 0;
	
	if (type==configs::t_table_named)
	{
		effect_options_descriptor* children = (effect_options_descriptor*)data;
		
		for (u32 i=0; i<count; i++)
		{
			if (children->id && key && xray::strings::compare(children->id, key) == 0)
			{
				return *children;
			}
			offset += children->get_num_used_bytes();
			children = (effect_options_descriptor*)((byte*)children + children->get_num_used_bytes());
		}
	}
	
	effect_options_descriptor& v = *new(data+offset)effect_options_descriptor;
	
	xray::memory::copy(data + offset + sizeof(v), strings::length(key)+1, key, strings::length(key)+1);
	
	count++;
	
	v.id = pcstr(data + offset + sizeof(v));
	v.type = configs::t_table_named;
	v.bytes = sizeof(v) + strings::length(key) + 1;
	v.data  = data + offset + v.bytes;
	v.count = 0;
	v.destroyer = 0;
	
	return v;
}

} // namespace render
} // namespace xray
