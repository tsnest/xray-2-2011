////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_test_utils.h"
#include "game_resman_test_allocator.h"
#include "game_resman_test_resource.h"

namespace xray {
namespace resources {

extern math::random32		s_random;

void   log_test_resource (log_test_resource_enum flag, test_resource * resource)
{
	static u32 iteration				=	0;
	++iteration;

	fixed_string<32>					sign;
	if ( flag == log_test_resource_created )
		sign						=	"+";
	else if ( flag == log_test_resource_zero_reference )
		sign						=	"0 ref";
	else if ( flag == log_test_resource_destroyed )
		sign						=	"~";
	else if ( flag == log_test_resource_no_user_reference )
		sign						=	"1 ref";
	else
		NOT_IMPLEMENTED					();

	u32 resource_integer_name		=	resource->int_name();
	XRAY_UNREFERENCED_PARAMETER			(resource_integer_name);

	LOGI_INFO							("grm", 
										 "%d) %s %s, size %d", 
										 iteration, 
										 sign.c_str(), 
										 resource->name(), 
										 resource->size());
}

pcstr   test_allocator_name (class_id_enum const class_id)
{
	if ( class_id == test_resource_class1 )
		return						"A";
	else if ( class_id == test_resource_class2 )
		return						"B";
	else if ( class_id == test_resource_class3 )
		return						"C";
	else
		return						"";
}

static
log_string_type	  logging_name_for_query (query_result * query)
{
	log_string_type					out_result;
	out_result.assignf				("'%s%s %s' [quid %d]", 
									query->get_requested_path(), 
									test_allocator_name(query->get_class_id()), 
									query->is_resource_with_quality() ? "Q" : "",
									query->uid());

	return							out_result.c_str();
}

log_string_type   log_string (resource_base * resource)
{
	if ( query_result * query = resource->cast_query_result() )
		return						logging_name_for_query(query);

	return							resource->log_string();
}
	
} // namespace resources
} // namespace xray