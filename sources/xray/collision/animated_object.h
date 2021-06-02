////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_ANIMATED_OBJECT_H_INCLUDED
#define XRAY_COLLISION_ANIMATED_OBJECT_H_INCLUDED

#include <xray/collision/api.h>
#include <xray/collision/bone_collision_data.h>
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace collision {

class geometry;

#pragma warning( push )
#pragma warning( disable : 4231 )
template class XRAY_CORE_API buffer_vector< bone_collision_data >;
#pragma warning( pop )

class XRAY_COLLISION_API animated_object
{
public:
			animated_object			(
				configs::binary_config_value const& config,
				animation::skeleton_ptr const& model_skeleton,
				u32 const bones_count,
				memory::base_allocator* allocator
			);
			~animated_object		( );

			void	update			( float4x4 const* const bones_matrices_begin, float4x4 const* const bones_matrices_end );
			void	destroy			( memory::base_allocator* allocator );
	inline	u32		get_bones_count	( ) const { return m_geometries_data.size(); }
			math::aabb	get_aabb	( ) const;
			geometry*	get_geometry( ) const { return m_geometry; } 
			void	draw_collision	( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const;
			
			float3	get_random_surface_point( u32 const current_time ) const;
			float3	get_head_bone_center	( ) const;
			float3	get_eyes_direction		( ) const;

private:
	typedef buffer_vector< bone_collision_data > bone_collisions_type;

private:
	bone_collisions_type			m_geometries_data;
	geometry*						m_geometry;
	u32								m_head_bone_index;
}; // class animated_object

typedef	resources::resource_ptr < animated_object, resources::unmanaged_intrusive_base > animated_object_ptr;

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_ANIMATED_OBJECT_H_INCLUDED
