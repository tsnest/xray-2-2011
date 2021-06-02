////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////


#pragma make_public (xray::configs::lua_config_value)
#pragma make_public (xray::math::color)

#pragma make_public (xray::math::float3)
#pragma make_public (xray::math::float4x4)
#pragma make_public (xray::math::int2)
#pragma make_public (xray::math::float2)
#pragma make_public (xray::math::aabb)

#include <xray/collision/object.h>
#pragma make_public (xray::collision::object)

#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/world.h>
#pragma make_public (xray::render::editor::renderer)
#pragma make_public (xray::render::debug::renderer)
#pragma make_public (xray::render::world)
#pragma make_public (xray::resources::unmanaged_resource)

#pragma make_public (xray::render::base_scene)
#pragma make_public (xray::render::base_scene_view)
#pragma make_public (xray::render::base_output_window)
 
#pragma make_public (xray::editor_base::allocator_type)
#pragma make_public (xray::collision::space_partitioning_tree)
#pragma make_public (xray::collision::ray_object_result)

#include <xray/animation/camera_follower.h>
#pragma make_public	(xray::camera_effector)

#pragma make_public(xray::editor_base::property_holder)
#pragma make_public(xray::editor_base::editor_intrusive_base)
#pragma make_public(xray::threading::single_threading_policy)
#pragma make_public(xray::editor_base::property_holder_collection)
#pragma make_public(xray::editor_base::property_holder_holder)

