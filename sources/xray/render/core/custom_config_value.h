////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_H_INCLUDED
#define XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_H_INCLUDED

#include <xray/render/core/static_type.h>

namespace xray {
namespace render {

bool per_bytes_equal(u8 const* a_ptr, u8 const* b_ptr, u32 const num_comparision);

class custom_config_value 
{
public:
	typedef custom_config_value const* const_iterator;
	
public:
	custom_config_value						();
	u32							size		() const;
	const_iterator				begin		() const;
	const_iterator				end			() const;
	custom_config_value const&	operator []	(pcstr key) const;
	bool						operator <	(u32 in_crc) const;
	pcvoid						get_data_ptr() const;
	bool						value_exists(pcstr key) const;
	
	template<class T> operator T() const;
	
	template<class config_value_type> bool operator == (config_value_type const& right) const;
private:
	friend struct custom_config;
	void call_data_destructor	() const;
public:
	pcstr				id;
	pcvoid				data;
	u32					id_crc;
	u16					type;
	u16					count;
	pcvoid				destroyer;
}; // class custom_config_value

} // namespace render
} // namespace xray

#include <xray/render/core/custom_config_value_inline.h>

#endif // #ifndef XRAY_RENDER_CORE_CUSTOM_CONFIG_VALUE_H_INCLUDED