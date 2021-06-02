////////////////////////////////////////////////////////////////////////////
//	Created		: 09.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_resource_flags.h>
#include <xray/resources_managed_resource.h>
#include <xray/resources_unmanaged_resource.h>

namespace xray {
namespace resources {

resource_flags::resource_flags			(resource_flags_enum flags) 
	:	m_flags(flags), 
		vfs_association	(flags & 
			(resource_flag_is_managed_resource |
			 resource_flag_is_query_result |
			 resource_flag_is_unmanaged_resource)
						)
{
}

managed_resource *   resource_flags::cast_managed_resource ()
{
	if ( has_flags(resource_flag_is_managed_resource) )
		return								static_cast_checked<managed_resource*>(this);

	return									NULL;
}

unmanaged_resource *   resource_flags::cast_unmanaged_resource ()
{
	if ( has_flags(resource_flag_is_unmanaged_resource) )
		return								static_cast_checked<unmanaged_resource*>(this);

	return									NULL;
}

query_result *   resource_flags::cast_query_result ()
{
	if ( has_flags(resource_flag_is_query_result) )
		return								static_cast_checked<query_result*>(this);

	return									NULL;
}

query_result const *   resource_flags::cast_query_result () const
{
	if ( has_flags(resource_flag_is_query_result) )
		return								static_cast_checked<query_result const*>(this);

	return									NULL;
}

base_of_intrusive_base *   resource_flags::cast_base_of_intrusive_base ()
{
	if ( managed_resource * const resource = cast_managed_resource() )
		return								resource;
	else if ( unmanaged_resource * const unmanaged_resource = cast_unmanaged_resource() )
		return								unmanaged_resource;

	return									NULL;
}

positional_unmanaged_resource *   resource_flags::cast_positional_unmanaged_resource ()
{
	if ( has_flags(resource_flag_is_positional_unmanaged) )
		return								static_cast_checked<positional_unmanaged_resource *>(this);
	
	return									NULL;
}

resource_base *		resource_flags::cast_resource_base	() 
{ 
	return									static_cast_checked<resource_base *>(this); 
}

resource_base const *	resource_flags::cast_resource_base	() const 
{ 
	return									static_cast_checked<resource_base const *>(this); 
}


} // namespace resources
} // namespace xray

