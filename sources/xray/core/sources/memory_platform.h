////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef MEMORY_PLATFORM_H_INCLUDED
#define MEMORY_PLATFORM_H_INCLUDED

namespace xray {

namespace memory {
namespace platform {

struct region {
	u64		size;
	pvoid	address;
	pvoid	data;
}; // struct region

inline bool operator< ( region const& left, region const& right )
{
	return	left.size < right.size;
}

typedef buffer_vector<region>					regions_type;

void	allocate_arenas							(
			u64 const reserved_memory_size,
			u64 const reserved_address_space,
			regions_type& arenas,
			region& managed_arena,
			region& unmanaged_arena
		);
void	free_region								( pvoid buffer, u64 buffer_size );

} // namespace platform
} // namespace memory
} // namespace xray

#endif // #ifndef MEMORY_PLATFORM_H_INCLUDED