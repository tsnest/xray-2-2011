////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_COOK_H_INCLUDED
#define PARTICLE_SYSTEM_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include "particle_system.h"

namespace xray {
namespace particle {

class particle_system_cook : public resources::inplace_unmanaged_cook
{
public:
	particle_system_cook() : inplace_unmanaged_cook	(resources::particle_system_binary_class,
													 reuse_true,
			 										 use_resource_manager_thread_id,
			 										 use_resource_manager_thread_id)
	{
	}

	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook &	in_query, 
													 u32								file_size, 
													 u32 &								out_offset_to_file, 
													 bool								file_exist)
	{
		XRAY_UNREFERENCED_PARAMETERS		(in_query, file_exist);
		out_offset_to_file				=	sizeof(particle_system);
		u32 const size					=	sizeof(particle_system) + file_size;
		return								mutable_buffer(UNMANAGED_ALLOC(char, size), size);
	}

	virtual void			deallocate_resource		(pvoid buffer) 
	{
		UNMANAGED_FREE						(buffer);
	}

	virtual void			create_resource			(resources::query_result_for_cook & in_out_query, 
													 mutable_buffer						in_out_unmanaged_resource_buffer)
	{
		particle_system * const system	=	new (in_out_unmanaged_resource_buffer.c_ptr()) particle_system;
		u32 data_size					=	in_out_unmanaged_resource_buffer.size() - sizeof(particle_system);
		pvoid const data_buffer			=	(pbyte)in_out_unmanaged_resource_buffer.c_ptr() + sizeof(particle_system);
		mutable_buffer load_buffer			(data_buffer, data_size);
		system->load_binary					(load_buffer);

		in_out_query.set_unmanaged_resource	(system, resources::unmanaged_memory, in_out_unmanaged_resource_buffer.size());
		in_out_query.finish_query			(result_success);
	}

	virtual void			destroy_resource		(resources::unmanaged_resource *	resource)
	{
		particle_system * const system	=	static_cast_checked<particle_system *>(resource);
		system->~particle_system			();
	}
};

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_COOK_H_INCLUDED