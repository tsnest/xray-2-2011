////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_INSTANCE_COOK_H_INCLUDED
#define PARTICLE_SYSTEM_INSTANCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace particle {

struct world;

class particle_system;
class particle_emitter;

class particle_system_instance_cook :	
	public resources::translate_query_cook,
	public boost::noncopyable
{
	typedef resources::translate_query_cook	super;
	
public:
	particle_system_instance_cook							();
	virtual			~particle_system_instance_cook		( ) {}
	
	virtual	void	translate_query			( resources::query_result_for_cook & parent );
	virtual void	delete_resource			( resources::resource_base* res );
	
private:
	void			on_sub_resources_loaded	( resources::queries_result & data, particle::world* p_world );
}; // class particle_system_instance_cook

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_INSTANCE_COOK_H_INCLUDED