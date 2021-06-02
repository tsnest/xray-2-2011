////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_WRAPPER_COOK_H_INCLUDED
#define PARTICLE_SYSTEM_WRAPPER_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace particle {

struct world;

class particle_system;
class particle_emitter;
class particle_action;


class particle_system_wrapper_cook :	
	public resources::translate_query_cook,
	public boost::noncopyable
{
	typedef resources::translate_query_cook	super;

public:
	particle_system_wrapper_cook							();
	virtual					~particle_system_wrapper_cook	() {}
	
	virtual	void			translate_query					(resources::query_result_for_cook & parent);
	void					on_resource_ready				(resources::queries_result & result);

	void					on_fs_iterators_ready			(resources::queries_result & result);
	void					request_convertion				(pcstr resource_path, resources::query_result_for_cook * parent);

	void					on_config_loaded				(resources::queries_result& result);
	virtual void			delete_resource					(resources::resource_base * resource);

}; // class particle_system_cooker

void register_particles_system_cooks		();
void unregister_particles_system_cooks		();

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_WRAPPER_COOK_H_INCLUDED