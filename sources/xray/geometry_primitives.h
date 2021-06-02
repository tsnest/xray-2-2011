////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRY_PRIMITIVES_H_INCLUDED
#define GEOMETRY_PRIMITIVES_H_INCLUDED

namespace xray {
namespace geometry_utils{

////////////////////////////////////////////////////////////////////////////
// Oriented Bounding Box
////////////////////////////////////////////////////////////////////////////

namespace obb {

	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];

} // namespace obb

////////////////////////////////////////////////////////////////////////////
// Rectangle
////////////////////////////////////////////////////////////////////////////
namespace rectangle {

	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];

} // namespace Rectangle

////////////////////////////////////////////////////////////////////////////
// Ellipsoid
////////////////////////////////////////////////////////////////////////////
namespace ellipsoid {
	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[ ];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} // namespace ellipsoid

////////////////////////////////////////////////////////////////////////////
// Cone
////////////////////////////////////////////////////////////////////////////
namespace cone {
	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} // namespace cone


////////////////////////////////////////////////////////////////////////////
// Ellipse 
////////////////////////////////////////////////////////////////////////////
namespace ellipse {
	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} // namespace ellipse
////////////////////////////////////////////////////////////////////////////
// Cylinder 
////////////////////////////////////////////////////////////////////////////
namespace cylinder {
	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} // namespace ellipse

////////////////////////////////////////////////////////////////////////////
// Circle 
////////////////////////////////////////////////////////////////////////////
namespace circle {
	extern XRAY_CORE_API const u32	vertex_count;
	extern XRAY_CORE_API const float	vertices[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} // namespace ellipse

////////////////////////////////////////////////////////////////////////////
// Cube solid
////////////////////////////////////////////////////////////////////////////
namespace cube_solid {
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count; 
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];
} //namespace cube_solid 


////////////////////////////////////////////////////////////////////////////
// Rectangle solid
////////////////////////////////////////////////////////////////////////////
namespace rectangle_solid {

	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count;
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];

} // namespace Rectangle

////////////////////////////////////////////////////////////////////////////
// Cone solid
////////////////////////////////////////////////////////////////////////////
namespace cone_solid {
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count; 
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];
} // namespace cone_solid


////////////////////////////////////////////////////////////////////////////
// Cylinder
////////////////////////////////////////////////////////////////////////////
namespace cylinder_solid {
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count; 
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];
} //namespace cylinder_solid 

////////////////////////////////////////////////////////////////////////////
// Ellipsoid solid
////////////////////////////////////////////////////////////////////////////
namespace ellipsoid_solid {
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count; 
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];
} //namespace ellipsoid_solid 

namespace octahedron_solid {
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices[];

	extern XRAY_CORE_API const u32 face_count; 
	extern XRAY_CORE_API const u32 index_count; 
	extern XRAY_CORE_API const u16 faces[];

	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} //namespace octahedron_solid

namespace line_ellipsoid{
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices_xy[];
	extern XRAY_CORE_API const float vertices_yz[];
	extern XRAY_CORE_API const float vertices_xz[];
	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} //line_ellipsoid

namespace line_arc{
	extern XRAY_CORE_API const u32 vertex_count;
	extern XRAY_CORE_API const float vertices_xy[];
	extern XRAY_CORE_API const u32	pair_count;
	extern XRAY_CORE_API const u16	pairs[];
} //line_arc

bool generate_torus			( xray::vectora< math::float3 >& vertices, xray::vectora< u16 >& indices, math::float4x4 transform,
									 float outer_raduius,  float inner_raduius, u16 outer_segments, u16 inner_segments );

} // namespace geometry_utils
} // namespace xray

#endif // #ifndef GEOMETRY_PRIMITIVES_H_INCLUDED