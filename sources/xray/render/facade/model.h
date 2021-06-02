////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_VISUAL_H_INCLUDED
#define XRAY_RENDER_BASE_VISUAL_H_INCLUDED

#include <xray/collision/geometry_instance.h>
#include <xray/animation/skeleton.h>
#include <xray/physics/collision_shapes.h>
#include <xray/collision/geometry.h>

namespace xray {

namespace collision {
	class animated_object;
} // namespace collision

namespace render {

struct model_locator_item
{
	string32	m_name;
	float4x4	m_offset;
	u16			m_bone;
};

class render_model_instance : public resources::unmanaged_resource 
{
public:
	virtual void		update		( )		{}
	virtual bool		get_locator	( pcstr /*locator_name*/, model_locator_item& /*result*/ ) const {return false;}
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( render_model_instance )
}; // class render_model_instance

typedef	resources::resource_ptr<
	render_model_instance,
	resources::unmanaged_intrusive_base
> render_model_instance_ptr;

class static_model_instance : public resources::unmanaged_resource 
{
public:
	render::render_model_instance_ptr	m_render_model;
	collision::geometry_ptr				m_collision_geom;
};

typedef	resources::resource_ptr<
	static_model_instance,
	resources::unmanaged_intrusive_base
> static_model_ptr;

class skeleton_model_instance : public resources::unmanaged_resource
{
public:
	render_model_instance_ptr			m_render_model;
	animation::skeleton_ptr				m_skeleton;
}; // class skeleton_model_instance

typedef	resources::resource_ptr<
	skeleton_model_instance,
	resources::unmanaged_intrusive_base >
skeleton_model_ptr;

class terrain_model_instance : public resources::unmanaged_resource 
{
public:
	render::render_model_instance_ptr		m_render_model;
	collision::geometry_instance*			m_collision_geom;
	physics::bt_collision_shape_ptr			m_bt_collision_shape;
};

typedef	resources::resource_ptr<
	terrain_model_instance,
	resources::unmanaged_intrusive_base
> terrain_model_ptr;



// SpeedTree

struct speedtree_tree_base: public resources::unmanaged_resource 
{
	speedtree_tree_base():m_bbox(math::create_zero_aabb()){}
	xray::math::aabb					m_bbox;
}; // struct speedtree_tree_base

typedef	resources::resource_ptr<
	speedtree_tree_base,
	resources::unmanaged_intrusive_base
> speedtree_tree_ptr;


class speedtree_instance: public resources::unmanaged_resource 
{
public:
		speedtree_instance(render::speedtree_tree_ptr in_speedtree_tree_ptr):m_speedtree_tree_ptr(in_speedtree_tree_ptr){}

	xray::math::float4x4				m_transform;
	render::speedtree_tree_ptr			m_speedtree_tree_ptr;
	collision::geometry_ptr				m_collision_geometry;
};

typedef	resources::resource_ptr<
	speedtree_instance,
	resources::unmanaged_intrusive_base
> speedtree_instance_ptr;


class skin : public resources::unmanaged_resource 
{
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( skin )
}; // class skin

typedef	resources::resource_ptr<
	skin,
	resources::unmanaged_intrusive_base
> skin_ptr;

struct animated_model_instance : public resources::unmanaged_resource
{
	render_model_instance_ptr			m_model;
	skin_ptr							m_skin;
	collision::animated_object*			m_bounding_collision;
}; // struct animated_model_instance

typedef	resources::resource_ptr<
	animated_model_instance,
	resources::unmanaged_intrusive_base 
>animated_model_instance_ptr;

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_VISUAL_H_INCLUDED
