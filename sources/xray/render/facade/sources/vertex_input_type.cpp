////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/vertex_input_type.h>

namespace xray {
namespace render {

enum_vertex_input_type string_to_vertex_input_type(pcstr name)
{
	if		(strings::equal(name,"null"))					return null_vertex_input_type;
	else if (strings::equal(name,"static"))					return static_mesh_vertex_input_type;
	else if (strings::equal(name,"skeletal"))				return skeletal_mesh_vertex_input_type;
	else if (strings::equal(name,"particle"))				return particle_vertex_input_type;
	else if (strings::equal(name,"particle_subuv"))			return particle_subuv_vertex_input_type;
	else if (strings::equal(name,"particle_beamtrail"))		return particle_beamtrail_vertex_input_type;
	else if (strings::equal(name,"decal"))					return decal_vertex_input_type;
	else if (strings::equal(name,"lensflares"))				return lensflares_vertex_input_type;
	else if (strings::equal(name,"terrain"))				return terrain_vertex_input_type;
	else if (strings::equal(name,"grassmesh"))				return grassmesh_vertex_input_type;
	else if (strings::equal(name,"grassbillboard"))			return grassbillboard_vertex_input_type;
	else if (strings::equal(name,"speedtree_branch"))		return speedtree_branch_vertex_input_type;
	else if (strings::equal(name,"speedtree_frond"))		return speedtree_frond_vertex_input_type;
	else if (strings::equal(name,"speedtree_leafmesh"))		return speedtree_leafmesh_vertex_input_type;
	else if (strings::equal(name,"speedtree_leafcard"))		return speedtree_leafcard_vertex_input_type;
	else if (strings::equal(name,"speedtree_billboard"))	return speedtree_billboard_vertex_input_type;
	else if (strings::equal(name,"post_process"))			return post_process_vertex_input_type;
	else												
	{
		R_ASSERT(0, "Unknown vertex input type name '%s'.", name);
		return unknown_vertex_input_type;
	}
}

pcstr vertex_input_type_to_string(enum_vertex_input_type type)
{
	switch (type)
	{
		case null_vertex_input_type:						return "null";
		case static_mesh_vertex_input_type:					return "static";
		case skeletal_mesh_vertex_input_type:				return "skeletal";
		case particle_vertex_input_type:					return "particle";
		case particle_subuv_vertex_input_type:				return "particle_subuv";
		case particle_beamtrail_vertex_input_type:			return "particle_beamtrail";
		case decal_vertex_input_type:						return "decal";
		case lensflares_vertex_input_type:					return "lensflares";
		case terrain_vertex_input_type:						return "terrain";
		case grassmesh_vertex_input_type:					return "grassmesh";
		case grassbillboard_vertex_input_type:				return "grassbillboard";
		case speedtree_branch_vertex_input_type:			return "speedtree_branch";
		case speedtree_frond_vertex_input_type:				return "speedtree_frond";
		case speedtree_leafmesh_vertex_input_type:			return "speedtree_leafmesh";
		case speedtree_leafcard_vertex_input_type:			return "speedtree_leafcard";
		case speedtree_billboard_vertex_input_type:			return "speedtree_billboard";
		case post_process_vertex_input_type:				return "post_process";
		default: NODEFAULT(return "unknown");
	};
}

} // namespace render
} // namespace xray

