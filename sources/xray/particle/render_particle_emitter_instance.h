////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PARTICLE_RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED
#define XRAY_PARTICLE_RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED

#include <xray/particle/particle_data_type.h>

namespace xray {
namespace particle {

struct XRAY_NOVTABLE render_particle_emitter_instance {
	virtual	void	change_material			(
						resources::unmanaged_resource_ptr const& material
					) = 0;
	virtual	void	update_render_buffers	(
						enum_particle_data_type data_type,
						bool use_subuv,
						u32 max_num_particles,
						u32 beamtrail_parameters_num_sheet
					) = 0;
	virtual	void	set_transform			( math::float4x4 const& transform ) = 0;
	virtual	void	set_aabb				( math::aabb const& bbox ) = 0;
	
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( render_particle_emitter_instance )
}; // struct render_particle_emitter_instance

} // namespace particle
} // namespace xray

#endif // #ifndef XRAY_PARTICLE_RENDER_PARTICLE_EMITTER_INSTANCE_H_INCLUDED