////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_USER_RENDER_MESH_H_INCLUDED
#define XRAY_RENDER_ENGINE_USER_RENDER_MESH_H_INCLUDED

#include "render_model.h"
#include <xray/render/facade/material_effects_instance_cook_data.h>

namespace xray {
namespace render {

class untyped_buffer;

class user_render_surface: public render_surface 
{
	typedef render_surface				super;
public:
	virtual void		load_from_chunk_data	( memory::chunk_reader& r ) = 0;

protected:
	void				material_ready			( resources::queries_result& data, render::material_effects_instance_cook_data* cook_data, pstr material_name );
};

class user_render_surface_editable: public user_render_surface 
{
	typedef user_render_surface				super;
public:
	virtual void		load_from_chunk_data	( memory::chunk_reader& r );
	untyped_buffer*		m_vb;
};

class user_render_surface_wire: public user_render_surface 
{
	typedef user_render_surface				super;
public:
	virtual void		load_from_chunk_data	( memory::chunk_reader& r );
};

class user_render_model_instance : public render_model_instance_impl
{
	typedef render_model_instance_impl		super;
public:
	virtual math::aabb	get_aabb					( ) { return m_surface->m_aabbox; }
			void		assign_surface				( user_render_surface* surface ); 
	virtual void		get_surfaces				( render_surface_instances&, bool visible_only );

	user_render_surface*		m_surface;
	render_surface_instance		m_surface_instance;
};

typedef	resources::resource_ptr<
	user_render_model_instance,
	resources::unmanaged_intrusive_base
> user_mesh_instance_ptr;

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_ENGINE_USER_RENDER_MESH_H_INCLUDED