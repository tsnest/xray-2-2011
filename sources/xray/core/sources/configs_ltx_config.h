////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_LTX_H_INCLUDED
#define CONFIG_LTX_H_INCLUDED

#include <xray/configs.h>

namespace xray {
namespace core {
namespace configs {

class binary_config;

void			parse_string_data	( memory::reader F, memory::base_allocator* allocator, pbyte* dest, u32& dest_size );
binary_config*	create_from_ltx_text( pcstr text, memory::base_allocator* allocator, pcstr file_name );

} // namespace configs
} // namespace core
} // namespace xray

#ifdef MASTER_GOLD
#error file must not be used in MASTER_GOLD
#endif

#endif // #ifndef CONFIG_LTX_H_INCLUDED