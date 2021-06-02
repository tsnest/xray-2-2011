////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_WORLD_COOKER_H_INCLUDED
#define PARTICLE_WORLD_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace particle {

class particle_world_cooker : public resources::unmanaged_cook
{
public:
	particle_world_cooker							();
	virtual	mutable_buffer	allocate_resource		(resources::query_result_for_cook& in_query, const_buffer raw_file_data, bool file_exist);
	virtual void			deallocate_resource		(pvoid buffer);
	virtual void			create_resource			(resources::query_result_for_cook& in_out_query, const_buffer raw_file_data, mutable_buffer in_out_unmanaged_resource_buffer);
	virtual void			destroy_resource		(resources::unmanaged_resource* resource);
}; // class particle_world_cooker

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_WORLD_COOKER_H_INCLUDED