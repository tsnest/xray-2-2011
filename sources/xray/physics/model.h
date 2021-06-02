////////////////////////////////////////////////////////////////////////////
//	Created		: 18.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PHYSICS_MODEL_H_INCLUDED
#define XRAY_PHYSICS_MODEL_H_INCLUDED

#include <xray/animation/skeleton.h>

namespace xray {

namespace collision {
	class animated_object;
	class object;
} // namespace collision

namespace physics {

enum model_type
{
	model_type_dynamic,
	model_type_static,
	model_type_static_none = model_type_static,
};

struct shell;

struct animated_model_instance : public resources::unmanaged_resource
{
	animation::skeleton_ptr			m_skeleton;
	collision::animated_object*		m_collision;
	shell*							m_physics;

}; // struct animated_model_instance

typedef	resources::resource_ptr<
			animated_model_instance,
			resources::unmanaged_intrusive_base >
		animated_model_instance_ptr;

struct solid_model_instance
{
	collision::object*				m_collision;
}; // struct solid_model_instance

} // namespace physics
} // namespace xray

#endif // #ifndef XRAY_PHYSICS_MODEL_H_INCLUDED