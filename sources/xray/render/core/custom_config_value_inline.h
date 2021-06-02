////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_INLINE_H_INCLUDED
#define XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_INLINE_H_INCLUDED

namespace xray {
namespace render {

inline custom_config_value::custom_config_value	():
	id		(NULL),
	data	(NULL),
	id_crc	(0),
	type	(0),
	count	(0),
	destroyer(0)
{}

inline u32 custom_config_value::size() const 
{ 
	return u32(end() - begin()); 
}

inline custom_config_value::const_iterator custom_config_value::begin() const
{
	R_ASSERT			(type==xray::configs::t_table_named);
	return				pointer_cast<const_iterator>(data);
}

inline custom_config_value::const_iterator custom_config_value::end() const
{
	R_ASSERT			(type==xray::configs::t_table_named);
	return				pointer_cast<const_iterator>(data) + count;
}

inline bool custom_config_value::operator < (u32 in_crc) const
{
	return id_crc < in_crc;
}

template<class config_value_type> 
inline bool custom_config_value::operator == (config_value_type const& right) const
{
	custom_config_value const& left = *this;
	if (left.id_crc!=right.id_crc || left.count!=right.count)
		return false;
	
	if (left.type==xray::configs::t_table_named && (right.type==xray::configs::t_table_named || right.type==xray::configs::t_table_indexed))
	{
		const_iterator left_it	= left.begin();
		const_iterator left_e	= left.end();
		config_value_type::const_iterator right_it = right.begin();
		
		for (; left_it!=left_e; ++left_it, ++right_it)
			if (!(*left_it==*right_it))
				return false;
	}
	else
	{
		if (left.type==static_type::get_type_id<pcstr>())
		{
			if (strings::compare(pcstr(left.data),pcstr(pcvoid(right.data)))!=0)
				return false;
		}
		else
		{
			if (left.count <= sizeof(u32))
			{
				if (left.data!=right.data)
					return false;
			}
			else
			{
				if (!per_bytes_equal(pbyte(left.data), pbyte(pcvoid(right.data)), left.count))
					return false;
			}
		}
	}
	
	return true;
}

template<class T> 
inline custom_config_value::operator T() const
{
	ASSERT_CMP(static_type::get_type_id<T>(), ==, type);
	
	if (identity(sizeof(T) <= sizeof(u32)) || type==static_type::get_type_id<pcstr>())
		return *(T*)&data;
	else
		return *(T*)data;
}

template<> 
inline custom_config_value::operator float() const
{
	bool is_integer_type = 
		type == static_type::get_type_id<u8>()  || type == static_type::get_type_id<s8>()  ||
		type == static_type::get_type_id<u16>() || type == static_type::get_type_id<s16>() ||
		type == static_type::get_type_id<u32>() || type == static_type::get_type_id<s32>() ||
		type == static_type::get_type_id<u64>() || type == static_type::get_type_id<s64>();

	if( is_integer_type)
		return (float)u64(data);

	ASSERT_CMP(static_type::get_type_id<float>(), ==, type);

	if (identity(sizeof(float) <= sizeof(u32)) || type==static_type::get_type_id<pcstr>())
		return *(float*)&data;
	else
		return *(float*)data;
}

template<> 
inline custom_config_value::operator double() const
{
	bool is_integer_type = 
		type == static_type::get_type_id<u8>()  || type == static_type::get_type_id<s8>()  ||
		type == static_type::get_type_id<u16>() || type == static_type::get_type_id<s16>() ||
		type == static_type::get_type_id<u32>() || type == static_type::get_type_id<s32>() ||
		type == static_type::get_type_id<u64>() || type == static_type::get_type_id<s64>();

	if( is_integer_type)
		return (double)u64(data);

	ASSERT_CMP(static_type::get_type_id<double>(), ==, type);

	if (identity(sizeof(double) <= sizeof(u32)) || type==static_type::get_type_id<pcstr>())
		return *(double*)&data;
	else
		return *(double*)data;
}

inline pcvoid custom_config_value::get_data_ptr() const
{
	if (count <= sizeof(u32) || type==static_type::get_type_id<pcstr>())
		return &data;
	else
		return data;
}

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_INLINE_H_INCLUDED