////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UNMANAGED_ALLOCATION_RESOURCE_H_INCLUDED
#define UNMANAGED_ALLOCATION_RESOURCE_H_INCLUDED

namespace xray {
namespace resources {

struct unmanaged_allocation_resource :
	public resources::unmanaged_resource,
	private boost::noncopyable
{
	unmanaged_allocation_resource	( u32 resource_size ) :
		size						( resource_size )
	{
	}
	
	u32		size;
#pragma warning( push )
#pragma warning( disable : 4200 )
	u8	buffer[];
#pragma warning( pop )
}; // struct unmanaged_allocation_resource

typedef	resources::resource_ptr < unmanaged_allocation_resource, resources::unmanaged_intrusive_base > unmanaged_allocation_resource_ptr;

} // namespace resources
} // namespace xray

#endif // #ifndef UNMANAGED_ALLOCATION_RESOURCE_H_INCLUDED