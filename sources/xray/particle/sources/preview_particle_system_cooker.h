////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PREVIEW_PARTICLE_SYSTEM_COOKER_H_INCLUDED
#define PREVIEW_PARTICLE_SYSTEM_COOKER_H_INCLUDED

#ifndef MASTER_GOLD

#include <xray/resources_cook_classes.h>

namespace xray {
namespace particle {

struct world;

class particle_system;
class particle_emitter;
class particle_action;

class preview_particle_system_cooker :	
	public resources::translate_query_cook,
	public boost::noncopyable
{
	typedef resources::translate_query_cook	super;
	
public:
	preview_particle_system_cooker							();
	virtual					~preview_particle_system_cooker	() {}
	
	virtual	void			translate_query				( resources::query_result_for_cook & parent );
	virtual void			delete_resource				( resources::resource_base* res );

	void					on_lua_config_loaded		(resources::queries_result& result);
	
}; // class preview_particle_system_cooker

void register_preview_particle_system_cooker();
void unregister_preview_particle_system_cooker();

} // namespace particle
} // namespace xray

#else // #ifndef MASTER_GOLD
#	error do not include this header in MASTER_GOLD configurations!
#endif // #ifndef MASTER_GOLD

#endif // #ifndef PREVIEW_PARTICLE_SYSTEM_COOKER_H_INCLUDED