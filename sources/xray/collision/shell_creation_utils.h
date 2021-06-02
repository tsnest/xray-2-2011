////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_SHELL_CREATION_UTILS_H_INCLUDED
#define XRAY_COLLISION_SHELL_CREATION_UTILS_H_INCLUDED

namespace xray {
namespace collision {

class geometry_instance;
class geometry;
class bone_collision_data;

template < class ConfigValueType >
non_null< geometry >::ptr	new_composite_geometry_from_physics_shell_config(
		ConfigValueType	const& config, 
		buffer_vector< bone_collision_data >& geometries_data,
		memory::base_allocator* allocator
	);

template < class ConfigValueType >
bool read_transform	( ConfigValueType const& table, float4x4& result_transform, float3& result_scale );

template < class ConfigValueType >
non_null< geometry_instance >::ptr new_element_collision	(
		ConfigValueType const& collision_table, 
		float4x4 const& transform, 
		memory::base_allocator* allocator
	);

template < class ConfigValueType >
u32	calculate_composite_geometry_size_from_physics_shell_config	( ConfigValueType const& config );

template < class ConfigValueType >
u32	get_bones_count_from_physics_shell_config	( ConfigValueType const& config );

} // namespace collision
} // namespace xray

#include <xray/collision/shell_creation_utils_inline.h>

#endif // #ifndef XRAY_COLLISION_SHELL_CREATION_UTILS_H_INCLUDED