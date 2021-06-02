////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_OPTIONS_DESCRIPTOR_H_INCLUDED
#define EFFECT_OPTIONS_DESCRIPTOR_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <xray/render/core/static_type.h>

namespace xray {
namespace render {

struct effect_options_descriptor: public boost::noncopyable {
	effect_options_descriptor						();
	effect_options_descriptor						(pcvoid in_data, u16 size);
	~effect_options_descriptor						();
	
	u32				get_crc							() const;
	u32				get_data_memory_usage			(u32& need_bytes_to_align4, u32& last_align_value) const;
	u32				get_num_total_fields			() const;
	u32				get_num_used_bytes				() const;
	effect_options_descriptor& operator[]			(pcstr key);
	
	void call_destructors();
	
	template<typename T> effect_options_descriptor& operator = (T value)
	{
		// Next assignments.
		if (type != configs::t_table_named)
		{
			ASSERT_CMP(static_type::get_type_id<T>(), ==, type);
		}
		
		u16 type_size = sizeof(T);
		
		if (identity(type_size <= sizeof(u32)))
		{
			xray::memory::zero(&data,sizeof(data));
			T& v = *new(&data)T;
			v = value;
			count = sizeof(u32);
		}
		else
		{
			T& v = *new(data)T;
			v = value;
			if (type == configs::t_table_named) 
			{
				bytes += type_size;
			}
			count = type_size;
		}
		
		type = static_type::get_type_id<T>();
		
		new(&destroyer)destroy_data_helper<T const>();
		
		return *this;
	}

	template<> effect_options_descriptor& operator = <pcstr>	(pcstr value)
	{
		count = u16(strings::length(pcstr(value)) + 1);
		
		xray::memory::copy(data, count, value, count);
		
		if (type == configs::t_table_named) bytes += count;
		
		type = static_type::get_type_id<pcstr>();
		
		return *this;
	}
	
	//template<> effect_options_descriptor& operator = <effect_options_descriptor>	(effect_options_descriptor value) { return *this; }
	
	struct const_iterator
	{
		const_iterator() {}
		const_iterator(effect_options_descriptor const* const in_value): 
			child_index(0), 
			size(in_value->count), 
			value((effect_options_descriptor*)in_value->data) 
		{
			if (in_value->type!=configs::t_table_named)
				size = 0;
		}
		operator bool								() { return child_index < size; }
		void operator ++							() { child_index++; value = (effect_options_descriptor*)((byte*)value + value->get_num_used_bytes()); }
		effect_options_descriptor const& operator*	() { return *value; }
		effect_options_descriptor const* operator->	() { return value; }
		
		bool operator !=							(const_iterator const& it)
		{
			return child_index < it.size;
		}
	private:
		effect_options_descriptor* value;
		u32				  child_index;
		u32				  size;
	};
	
	// For const_iterator only.
	const_iterator begin() const
	{ 
		const_iterator it(this);
		return it; 
	}
	const_iterator end			()  const
	{ 
		const_iterator it(this);
		return it; 
	}
	
	pcstr	id;
	pcvoid  destroyer;
	pbyte	data;
	u32		bytes;
	u16		type;
	u16		count;
	u16		memory_size;
};
} // namespace render
} // namespace xray

#endif // #ifndef EFFECT_OPTIONS_DESCRIPTOR_H_INCLUDED