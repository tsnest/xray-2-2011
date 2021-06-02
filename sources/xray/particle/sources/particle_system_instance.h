////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SYSTEM_INSTANCE_H_INCLUDED
#define PARTICLE_SYSTEM_INSTANCE_H_INCLUDED

#include <boost/noncopyable.hpp>
#include <xray/resources_unmanaged_resource.h>
#include "particle_emitter_instance.h"

namespace xray {
namespace particle {

class particle_world;
class particle_system;

typedef	resources::resource_ptr<
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_ptr;

typedef xray::intrusive_list< 
	particle_emitter_instance, 
	particle_emitter_instance *, 
	&particle_emitter_instance::m_next, 
	threading::single_threading_policy, 
	size_policy> emitter_instance_list_type;

struct lod_entry {
	//particle_system_ptr?
	particle_system_ptr						m_template;
	emitter_instance_list_type				m_emitter_instance_list;
	
	float									m_distance;
	float									m_time_fade_in;
	float									m_time_fade_out;
	
}; // struct lod_entry

class particle_system_instance :
	public resources::unmanaged_resource,
	private boost::noncopyable
{
public:
	particle_system_instance( particle_world& particle_world );
	void	play			( );
	void	play			( xray::math::float4x4 const& transform );
	void	stop			( float time_to_stop );
	void	set_transform	( xray::math::float4x4 const& transform );

protected:
	enum { max_supported_lods = 10 };

public:
	lod_entry		m_lods[ max_supported_lods ];

protected:
	particle_world&	m_particle_world;
}; // class particle_system_instance

typedef	resources::resource_ptr<
	resources::unmanaged_resource,
	resources::unmanaged_intrusive_base
> particle_system_instance_ptr;

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_SYSTEM_INSTANCE_H_INCLUDED