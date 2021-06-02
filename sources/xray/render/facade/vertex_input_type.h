////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_VERTEX_INPUT_TYPE_H_INCLUDED
#define XRAY_RENDER_FACADE_VERTEX_INPUT_TYPE_H_INCLUDED

namespace xray {
namespace render {

enum enum_vertex_input_type
{
	null_vertex_input_type = 0,
	static_mesh_vertex_input_type,
	skeletal_mesh_vertex_input_type,
	particle_vertex_input_type,
	particle_subuv_vertex_input_type,
	particle_beamtrail_vertex_input_type,
	decal_vertex_input_type,
	lensflares_vertex_input_type,
	terrain_vertex_input_type,
	grassmesh_vertex_input_type,
	grassbillboard_vertex_input_type,
	speedtree_branch_vertex_input_type,
	speedtree_frond_vertex_input_type,
	speedtree_leafmesh_vertex_input_type,
	speedtree_leafcard_vertex_input_type,
	speedtree_billboard_vertex_input_type,
	post_process_vertex_input_type,
	wires_vertex_input_type,
	
	num_vertex_input_types,
	
	unknown_vertex_input_type
}; // enum enum_vertex_input_type

enum_vertex_input_type	string_to_vertex_input_type		(pcstr name);
pcstr					vertex_input_type_to_string		(enum_vertex_input_type type);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_VERTEX_INPUT_TYPE_H_INCLUDED
