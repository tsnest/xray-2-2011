////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_MODEL_COMPOSITE_H_INCLUDED
#define RENDER_MODEL_COMPOSITE_H_INCLUDED

#include "render_model_static.h"

namespace xray {
namespace render {

//shared
class composite_render_model :public render_model
{
	typedef render_model			super;
public:
	struct composite_item{
		static_render_model_ptr		m_model;
		float4x4					m_offset;
	};
	typedef render::vector<composite_item>	children;
	typedef children::iterator				children_it;

public:
						composite_render_model	( );
			void		append_model			( render_model_ptr v, float4x4 const& offset );

	u16					m_surfaces_count;
	children			m_children;
};

typedef	resources::resource_ptr<
			composite_render_model,
			resources::unmanaged_intrusive_base
		> composite_render_model_ptr;

class composite_render_model_instance : public render_model_instance_impl
{
	typedef render_model_instance_impl		super;

public:
						composite_render_model_instance( );
	virtual				~composite_render_model_instance( );
	virtual math::aabb	get_aabb					( ) {return m_original->m_aabbox;}
	virtual void		get_surfaces				( render_surface_instances& list, bool visible_only );
			void		assign_original				( composite_render_model_ptr v );
	virtual void		set_transform				( float4x4 const& transform );
protected:
	composite_render_model_ptr		m_original;
	u16								m_instances_count;
	render_surface_instance*		m_surface_instances;
	float4x4*						m_surface_matrices;
};

} // namespace render
} // namespace xray

#endif // #ifndef RENDER_MODEL_COMPOSITE_H_INCLUDED