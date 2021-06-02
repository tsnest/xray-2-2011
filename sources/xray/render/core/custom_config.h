////////////////////////////////////////////////////////////////////////////
//	Created		: 29.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef CUSTOM_CONFIG_H_INCLUDED
#define CUSTOM_CONFIG_H_INCLUDED

#include <xray/render/core/custom_config_value.h>

namespace xray {
namespace render {

struct effect_options_descriptor;

struct custom_config: public xray::resources::unmanaged_intrusive_base
{
	custom_config	 ();
	void destroy	 (custom_config* in_this);
	
	bool operator == (xray::configs::binary_config_value const& value)	{ return m_root == value; }
	bool operator == (custom_config_value const& value)	{ return m_root == value; }
	
	custom_config_value const& root	   () { return m_root; }
	operator custom_config_value const&() { return root(); }
	
	static u32 get_need_buffer_size	   (effect_options_descriptor const& v);
	
public:	
	bool					own_buffer;
	bool					call_destructors;
	
//private:
	custom_config_value		m_root;
};

typedef xray::intrusive_ptr < custom_config, custom_config, threading::simple_lock > custom_config_ptr;

custom_config_ptr	create_custom_config(effect_options_descriptor const& value, u32& out_data_crc, bool calc_data_crc = false);
custom_config_ptr	create_custom_config(custom_config_value const& value, u32& out_data_crc, bool calc_data_crc = false);
custom_config_ptr	create_custom_config(configs::binary_config_value const& value, u32& out_data_crc, bool calc_data_crc = false);

custom_config_ptr	create_custom_config(effect_options_descriptor const& value, xray::mutable_buffer& buffer, u32& out_data_crc, bool calc_data_crc = false);
custom_config_ptr	create_custom_config(custom_config_value const& value, xray::mutable_buffer& buffer, u32& out_data_crc, bool calc_data_crc = false);
custom_config_ptr	create_custom_config(configs::binary_config_value const& value, xray::mutable_buffer& buffer, u32& out_data_crc, bool calc_data_crc = false);

configs::binary_config_value copy_binary_config_value(configs::binary_config_value const& value, xray::mutable_buffer buffer, u32& out_data_crc, bool in_calc_data_crc);

u32					calc_config_memory_usage(effect_options_descriptor const& value);
u32					calc_config_memory_usage(custom_config_value const& value);
u32					calc_config_memory_usage(configs::binary_config_value const& value);

} // namespace render
} // namespace xray

#endif // #ifndef CUSTOM_CONFIG_H_INCLUDED