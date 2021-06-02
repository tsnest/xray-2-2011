////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_MODEL_AAA_H_INCLUDED
#define RENDER_MODEL_AAA_H_INCLUDED

#include <xray/render/facade/model.h>
#include "collision_object.h"
#include "material_effects.h"
#include <xray/render/facade/vertex_input_type.h>

namespace xray {
namespace render {

class res_geometry;
class resource_intrusive_base;

typedef	intrusive_ptr<
	res_geometry, 
	resource_intrusive_base, 
	threading::single_threading_policy
>	ref_geometry;

struct render_geometry
{
	ref_geometry	geom;

	u32			vertex_count;
	u32			index_count;
	u32			primitive_count;

	xray::fixed_string<64>	shading_group_name; // will be removed in MASTER_GOLD configuration
};//struct render_geometry

//not shared, unique
class render_surface : boost::noncopyable
{
public:
	render_surface			( ):m_aabbox(math::create_zero_aabb()){};
	virtual					~render_surface			( );
	enum_vertex_input_type	get_vertex_input_type	( )		{return m_vertex_input_type; }
	virtual void			load					( configs::binary_config_value const& properties, memory::chunk_reader& r );
			void			set_material_effects	( material_effects_instance_ptr mtl_instance_ptr, pcstr material_name );
			void			set_default_material	( );
	
	material_effects&		get_material_effects	( );
	
	enum_vertex_input_type				m_vertex_input_type;
	math::aabb							m_aabbox;
	render_geometry						m_render_geometry;
	material_effects_instance_ptr		m_materail_effects_instance;
};

class render_model_instance_impl;

struct render_surface_instance
{
	void							set_constants		( );

	render_surface*					m_render_surface;
	float4x4*						m_transform;
	render_model_instance_impl*		m_parent;
	bool							m_visible; // needed only for model_editor purposes
};

typedef vector< render_surface_instance* >	render_surface_instances;

class render_model : public resources::unmanaged_resource 
{
public:
						render_model		( );
	virtual	void		load_properties		( configs::binary_config_value const& properties );
	virtual	void		append_surface		( render_surface* ){};
	math::aabb			m_aabbox;
	virtual bool		get_locator			( pcstr locator_name, model_locator_item& result ) const;

protected:
	model_locator_item*			m_locators;
	u16							m_locators_count;
};

typedef	resources::resource_ptr<
	render_model,
	resources::unmanaged_intrusive_base
> render_model_ptr;

class render_model_instance_impl	: public render_model_instance
{
public:
						render_model_instance_impl	( );

	virtual	void		set_constants				( )		{}
	virtual math::aabb	get_aabb					( )		= 0;

	virtual void		get_surfaces				( render_surface_instances&, bool /*visible_only*/ ){};

	// TODO: make these protected
	
	collision_object<render_model_instance_impl>	m_collision_object;
	virtual void		set_transform				( float4x4 const& transform ) { m_transform = transform; }
	inline float4x4 const& transform				( ) const { return m_transform; }
protected:
 	float4x4										m_transform;
};

typedef	resources::resource_ptr<
	render_model_instance_impl,
	resources::unmanaged_intrusive_base
> render_model_instance_impl_ptr;

typedef vector< render_model_instance_impl_ptr >	render_model_instances;

} // namespace render 
} // namespace xray 

#endif // #ifndef RENDER_MODEL_AAA_H_INCLUDED
