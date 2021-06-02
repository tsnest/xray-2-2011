////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_HELPER_H_INCLUDED
#define RESOURCES_HELPER_H_INCLUDED

namespace xray {
namespace resources {
	class			fs_iterator;
} // namespace resources
} // namespace xray

#include "resources_macros.h"
#include <xray/resources.h>
#include <xray/resources_fs.h>
#include <xray/fs_path.h>

namespace xray {
namespace resources {

#define USER_ALLOCATOR	::xray::memory::g_resources_helper_allocator
#include <xray/std_containers.h>
#include <xray/unique_ptr.h>
#undef USER_ALLOCATOR

//----------------------------------------------------------
// general
//----------------------------------------------------------
class				queries_result;

typedef	fastdelegate::FastDelegate< void (bool result) >	store_callback;

class				query_result;

class raw_file_buffer_helper
{
public:
	raw_file_buffer_helper *	next;
	class_id_enum				resource_class;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_HELPER_H_INCLUDED