////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_RENDER_MESH_H_INCLUDED
#define SKELETON_RENDER_MESH_H_INCLUDED

#include "render_model.h"

namespace xray {
namespace render {

class skeleton_render_surface	: public render_surface
{
	typedef render_surface					super;
public:
					skeleton_render_surface	( );
					
	virtual	void	update					( render::vector<float4x4> const& bones ) = 0;

};

//shared
class shader_constant_host;
class skeleton_render_model : public render_model 
{
	typedef render_model		super;

public:
	typedef render::vector <skeleton_render_surface*>	children;
	typedef children::iterator							children_it;

public:
					skeleton_render_model	( );
					~skeleton_render_model	( );
			void	load_bones				( memory::reader& r );
	virtual	void	append_surface			( render_surface* );

			void	update					( render::vector<float4x4> const& bones );

	children const&	models					( ) const { return m_children; }

	shader_constant_host*			m_bones_matrices_shader_constant;
	render::vector< float4x4 >		m_inverted_bones_matrices_in_bind_pose;

	children						m_children;
};

typedef	resources::resource_ptr<
	skeleton_render_model,
	resources::unmanaged_intrusive_base
> skeleton_render_model_ptr;


class skeleton_render_model_instance : public render_model_instance_impl
{
	typedef render_model_instance_impl		super;
public:
							skeleton_render_model_instance	( );
	virtual					~skeleton_render_model_instance	( );
	virtual	void			get_surfaces			( render_surface_instances& list, bool visible_only );
	virtual	void			update					( );
	virtual	void			set_constants			( );
	virtual math::aabb		get_aabb				( ) {return math::create_identity_aabb()*2;} // TMP
			void			assign_original			( skeleton_render_model_ptr v );
			void			update_render_matrices	( float4x4 const* matrices, u32 count );
	virtual bool			get_locator				( pcstr locator_name, model_locator_item& result ) const;

	render::vector< float4x4 >			m_bones_matrices;
	skeleton_render_model_ptr			m_original;
	u16									m_instances_count;
	render_surface_instance*			m_surface_instances;
};

} // namespace render
} // namespace xray 

#endif // #ifndef SKELETON_RENDER_MESH_H_INCLUDED