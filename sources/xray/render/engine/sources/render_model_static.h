////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STATIC_RENDER_MESH_H_INCLUDED
#define STATIC_RENDER_MESH_H_INCLUDED

#include "render_model.h"

namespace xray {
namespace render {

class static_render_surface :public render_surface
{
	typedef render_surface	super;
public:
					static_render_surface	( );
	virtual			~static_render_surface	( ){};
	virtual void	load					( configs::binary_config_value const& properties, memory::chunk_reader& r );
};

//shared
class static_render_model :public render_model
{
	typedef render_model			super;
public:
	typedef render::vector<static_render_surface*>	children;
	typedef children::iterator						children_it;

public:
						static_render_model		( ){}
	virtual				~static_render_model	( );
	virtual	void		append_surface			( render_surface* );

	children					m_children;
};

typedef	resources::resource_ptr<
			static_render_model,
			resources::unmanaged_intrusive_base
		> static_render_model_ptr;


class static_render_model_instance : public render_model_instance_impl
{
	typedef render_model_instance_impl		super;

public:
						static_render_model_instance( );
	virtual				~static_render_model_instance( );
	virtual math::aabb	get_aabb					( ) {return m_original->m_aabbox;}
	virtual void		get_surfaces				( render_surface_instances& list, bool visible_only );
			void		assign_original				( static_render_model_ptr v );
	virtual bool		get_locator					( pcstr locator_name, model_locator_item& result ) const;

protected:
	static_render_model_ptr				m_original;
	u16									m_instances_count;
	render_surface_instance*			m_surface_instances;
};


} // namespace render 
} // namespace xray 

#endif // #ifndef STATIC_RENDER_MESH_H_INCLUDED