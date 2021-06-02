////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <boost/crc.hpp>

namespace xray {
namespace render {

custom_config::custom_config():
	own_buffer(false),
	call_destructors(true)
{}

void custom_config::destroy(custom_config* in_this)
{
	if (call_destructors)
		in_this->m_root.call_data_destructor();
	
	if (own_buffer)
		FREE(in_this);
}

u32 custom_config::get_need_buffer_size(effect_options_descriptor const& v)
{
	u32 num_fields			= v.get_num_total_fields();// + 1;
	u32 need_bytes_to_align = 0;
	u32 last_align_value	= 0;
	
	u32 num_bytes_for_data  = sizeof(custom_config) + num_fields * sizeof(custom_config_value) + v.get_data_memory_usage(need_bytes_to_align, last_align_value);
	num_bytes_for_data += need_bytes_to_align;// - last_align_value;
	
	return num_bytes_for_data;
}
	



template<class T> static u16 convert_type(u16) { return 0; }
template<> static u16 convert_type<custom_config_value>(u16 type)
{
	return type;
}
template<> static u16 convert_type<effect_options_descriptor>(u16 type)
{
	return type;
}
template<> static u16 convert_type<configs::binary_config_value>(u16 lua_type)
{
	switch (lua_type)
	{
		case configs::t_boolean:		return static_type::get_type_id<bool>();
		case configs::t_integer:		return static_type::get_type_id<s32>();
		case configs::t_float:			return static_type::get_type_id<float>();
		case configs::t_table_indexed:	return configs::t_table_named;
		case configs::t_table_named:	return configs::t_table_named;
		case configs::t_string:			return static_type::get_type_id<pcstr>();
		case configs::t_float2:			return static_type::get_type_id<math::float2>();
		case configs::t_float3:			return static_type::get_type_id<math::float3>();
		case configs::t_float4:			return static_type::get_type_id<math::float4>();
		default:
			NODEFAULT();
	}
	UNREACHABLE_CODE(return 0;);
}

template<class T, class T2> static void copy_destroyer(T const&, T2&){}
template<> static void copy_destroyer<custom_config_value, custom_config_value>(custom_config_value const& value, custom_config_value& item)			{ item.destroyer = value.destroyer;}
template<> static void copy_destroyer<effect_options_descriptor, custom_config_value>(effect_options_descriptor const& value, custom_config_value& item){ item.destroyer = value.destroyer;}


template<class T, class T2> static void construct(T const& value, T2& item, xray::mutable_buffer& data_buffer)
{
	item.type						= convert_type<T>(value.type);
	item.id							= value.id;
	item.data						= 0;
	item.count						= 0;
	item.id_crc						= 0;
	
	copy_destroyer(value, item);
	
	switch ( item.type ) 
	{
		case configs::t_table_named :
		case configs::t_table_indexed :
		{
			item.count				= value.count;
			item.data				= data_buffer.c_ptr();
			
			T2* items = (T2*)data_buffer.c_ptr();
			data_buffer	+= sizeof(T2) * value.count;
			
			for (T::const_iterator it = value.begin(); it!=value.end(); ++it)
			{
				T2* next_item = new(items++)T2;
				construct(*it, *next_item, data_buffer);
				
				if (it->id)
				{
					boost::crc_32_type			processor;
					processor.process_block		( it->id, it->id + xray::strings::length(it->id) );
					next_item->id_crc			= processor.checksum();
				}
			}
			
			break;
		}
		default:
			item.data  = value.data;
			item.count = value.count;
		break;
	}
}

template<class T> static void sort_by_crc(T& item)
{
	switch ( item.type ) {
		case configs::t_table_named:
		case configs::t_table_indexed: {
			T* items	= const_cast<T*>( static_cast<T const*>(static_cast<pcvoid>(item.data)) );
			
			struct predicate {
				static inline bool compare	( T const& left, T const& right )
				{
					return			left.id_crc < right.id_crc;
				}
			}; // struct predicate
			
			std::sort( items, items + item.count, &predicate::compare );
			
			for (u32 i=0; i<item.count; i++)
				sort_by_crc(*items++);
			
			break;
		}
	}
}

static void *align4(void *ptr)
{
	return (void*)((size_t(ptr) + 3) & ~3);
}

template<class T> static void fill_data(T const& value, xray::mutable_buffer& data_buffer)
{
	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
	{
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
			fill_data(*it, data_buffer);
	}
	else
	{
		pbyte ptr = pbyte(data_buffer.c_ptr());
		size_t add_types = pbyte(align4(ptr)) - ptr;
		if (add_types)
		{
			xray::memory::zero(data_buffer.c_ptr(), add_types);
			data_buffer += u32(add_types);
		}
		
		if (value.count > sizeof(u32) || value.type==static_type::get_type_id<pcstr>())
		{
			xray::memory::copy(data_buffer.c_ptr(), value.count, value.data, value.count);
			(pcvoid&)(value.data) = data_buffer.c_ptr();
			data_buffer += value.count;
		}
	}
	if (value.id)
	{
		u32 len = strings::length(value.id) + 1;
		xray::memory::copy(data_buffer.c_ptr(), len, value.id, len);
		(pcvoid&)(value.id) = pcstr(data_buffer.c_ptr());
		data_buffer += len;
	}
}

template<class T> static void fill_data_crc_buffer(T const& value, xray::mutable_buffer& buffer)
{
	if (value.id)
	{
		u32 len = strings::length(value.id) + 1;
		xray::memory::copy(buffer.c_ptr(), len, value.id, len);
		buffer += len;
	}

	if (value.type==configs::t_table_named)
	{
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
		{
			fill_data_crc_buffer(*it, buffer);
		}
	}
	else
	{
		pcvoid ptr = 0;
		
		if (value.count <= sizeof(u32) && value.type!=static_type::get_type_id<pcstr>())
			ptr = &value.data;
		else
			ptr = value.data;
		
		xray::memory::copy(buffer.c_ptr(), value.count, ptr, value.count);
		buffer += value.count;
	}
}

template<class T> static u32 get_data_crc_buffer_size(T const& value)
{
	u32 result = 0;
	
	if (value.id)
		result += strings::length(value.id) + 1;
	
	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
	{
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
			result += get_data_crc_buffer_size(*it);
	}
	else
	{
		result += value.count;
	}

	return result;
}

template<class T> static u32 calc_data_crc(T const& value)
{
	u32 buffer_size = get_data_crc_buffer_size(value);
	
	pcvoid mem_buffer = ALLOCA(buffer_size);
	xray::mutable_buffer buffer(pbyte(mem_buffer), buffer_size);
	
	fill_data_crc_buffer(value, buffer);
	
	boost::crc_32_type			processor;
	processor.process_block		(pbyte(mem_buffer), pbyte(mem_buffer) + buffer_size);
	
	return processor.checksum();
}

template<class T> static u32 get_num_config_fields_impl(T const& value)
{
	u32 result = 1;
	
	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
			result += get_num_config_fields_impl(*it);
	
	return result;
}

template<class T> static u32 get_num_config_fields(T const& value)
{
	u32 result = 0;
	
	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
			result += get_num_config_fields_impl(*it);
	
	return result;
}

template<class T> static u32 get_config_data_memory_usage(T const& value, u32& last_align_value)
{
	u32 result = 0;
	if (value.id)
	{
		result = strings::length(value.id) + 1;
	}
	
	if (value.type==configs::t_table_named || value.type==configs::t_table_indexed)
	{
		for (T::const_iterator it=value.begin(); it!=value.end(); ++it)
		{
			result += get_config_data_memory_usage(*it, last_align_value);
		}
	}
	else
	{
		if (value.count > sizeof(u32) || value.type==static_type::get_type_id<pcstr>())
			result += value.count;
		
		u32 const rem = result % 4;
		
		if (rem != 0)
		{
			last_align_value = 4 - rem;
			result += last_align_value;
		}
	}
	return result;
}


template<class T> static u32 calc_config_memory_usage_impl(T const& value)
{
	u32 last_align_value = 0;
	u32 result = sizeof(custom_config);
	result    += get_num_config_fields(value) * sizeof(custom_config_value);
	result	  += get_config_data_memory_usage(value, last_align_value);
	//result	  -= last_align_value;
	
	return result;
}

template<> static u32 calc_config_memory_usage_impl<effect_options_descriptor>(effect_options_descriptor const& value)
{
	return custom_config::get_need_buffer_size(value);
}

template<class T> static custom_config* create_custom_config_impl(T const& value, xray::mutable_buffer& data_buffer, u32& out_data_crc, bool is_calc_data_crc)
{
	custom_config* config = new(data_buffer.c_ptr())custom_config;
	data_buffer				+= sizeof(custom_config);
	
	construct(value, config->m_root, data_buffer);
	
	sort_by_crc(config->m_root);
	
	fill_data(config->m_root, data_buffer);
	
	if (is_calc_data_crc)
		out_data_crc = calc_data_crc(config->m_root);
	else
		out_data_crc = 0;
	
	return config;
}

template<class T> static custom_config_ptr create_custom_config_impl(T const& value, u32& out_data_crc, bool is_calc_data_crc)
{
	u32 num_bytes_for_data = calc_config_memory_usage_impl(value) * 2;
	
	xray::mutable_buffer b( ALLOC(byte, num_bytes_for_data), num_bytes_for_data );
	
	custom_config* config = create_custom_config_impl(value, b, out_data_crc, is_calc_data_crc);
	
	config->own_buffer = true;
	
	return config;
}

custom_config_ptr create_custom_config(effect_options_descriptor const& value, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, out_data_crc, is_calc_data_crc);
}

custom_config_ptr create_custom_config(custom_config_value const& value, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, out_data_crc, is_calc_data_crc);
}

custom_config_ptr create_custom_config(configs::binary_config_value const& value, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, out_data_crc, is_calc_data_crc);
}


custom_config_ptr create_custom_config(effect_options_descriptor const& value, xray::mutable_buffer& data_buffer, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, data_buffer, out_data_crc, is_calc_data_crc);
}

custom_config_ptr create_custom_config(custom_config_value const& value, xray::mutable_buffer& data_buffer, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, data_buffer, out_data_crc, is_calc_data_crc);
}

custom_config_ptr create_custom_config(configs::binary_config_value const& value, xray::mutable_buffer& data_buffer, u32& out_data_crc, bool is_calc_data_crc)
{
	return create_custom_config_impl(value, data_buffer, out_data_crc, is_calc_data_crc);
}

u32 calc_config_memory_usage(effect_options_descriptor const& value)
{
	return calc_config_memory_usage_impl(value);
}

u32 calc_config_memory_usage(custom_config_value const& value)
{
	return calc_config_memory_usage_impl(value);
}

u32 calc_config_memory_usage(configs::binary_config_value const& value)
{
	return calc_config_memory_usage_impl(value);
}


configs::binary_config_value copy_binary_config_value(configs::binary_config_value const& value, xray::mutable_buffer buffer, u32& out_data_crc, bool in_calc_data_crc)
{
	configs::binary_config_value* new_value = new(buffer.c_ptr())configs::binary_config_value;
	buffer						  += sizeof(configs::binary_config_value);
	
	construct(value, *new_value, buffer);
	
	sort_by_crc(*new_value);
	
	fill_data(*new_value, buffer);
	
	if (in_calc_data_crc)
		out_data_crc = calc_data_crc(*new_value);
	else
		out_data_crc = 0;
	
	return *new_value;
}

} // namespace render
} // namespace xray
