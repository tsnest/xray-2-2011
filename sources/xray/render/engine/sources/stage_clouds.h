////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_STAGE_CLOUDS_H_INCLUDED
#define XRAY_RENDER_ENGINE_STAGE_CLOUDS_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render {

class	light;
class	render_target;
class	resource_intrusive_base;
class	res_texture;
class	res_effect;
class	resource_intrusive_base;
class	untyped_buffer;
class	resource_intrusive_base;
class	res_geometry;

typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy>				ref_rt;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy>					ref_texture;
typedef	xray::resources::resource_ptr <xray::render::res_effect, xray::resources::unmanaged_intrusive_base>		ref_effect;
typedef	intrusive_ptr<untyped_buffer, resource_intrusive_base, threading::single_threading_policy>				untyped_buffer_ptr;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>				ref_geometry;



struct clouds_sliced_cube_geometry: public boost::noncopyable
{
	struct vertex_type
	{
		math::float4 position;
	}; // struct vertex_type
	
	clouds_sliced_cube_geometry	(u32 const num_cells);
	void					draw();
	
private:
	ref_declaration			m_vertext_declaration;
	untyped_buffer_ptr		m_vertex_buffer;
	untyped_buffer_ptr		m_index_buffer;
	u32	const				m_slices;
	u32						m_stride;
}; // struct clouds_sliced_cube_geometry




class stage_clouds: public stage
{
public:
										stage_clouds		(renderer_context* context);
										~stage_clouds		();
	virtual void						execute				();
			bool						is_effects_ready	() const;
private:
			
			ref_rt						m_3d_clouds_density_rt[2];
			ref_texture					m_3d_clouds_density_texture[2];
			
			ref_texture					m_3d_lockable_texture;
			
			ref_effect					m_clouds_effect;
			
			clouds_sliced_cube_geometry m_clouds_sliced_cube_geometry;
}; // class stage_clouds

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_STAGE_CLOUDS_H_INCLUDED