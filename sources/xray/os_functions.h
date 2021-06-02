////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_OS_FUNCTIONS_H_INCLUDED
#define XRAY_OS_FUNCTIONS_H_INCLUDED

#include <xray/macro_building.h>
#include <xray/type_extensions.h>

namespace xray {

namespace memory {
	class base_allocator;
} // namespace memory

namespace os {

XRAY_CORE_API void copy_to_clipboard	( pcstr buffer );
XRAY_CORE_API pstr paste_from_clipboard	( xray::memory::base_allocator& allocator );
XRAY_CORE_API void update_clipboard		( pcstr const string );
XRAY_CORE_API bool is_clipboard_empty	( );

} // namespace os
} // namespace xray

#endif // #ifndef XRAY_OS_FUNCTIONS_H_INCLUDED