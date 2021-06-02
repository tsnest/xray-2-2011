////////////////////////////////////////////////////////////////////////////
//	Created		: 08.06.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_VERTEX_COLORED_H_INCLUDED
#define XRAY_RENDER_ENGINE_VERTEX_COLORED_H_INCLUDED

namespace xray {
namespace render {

struct vertex_colored
{
	math::float3	position;
	math::color		color;

	inline vertex_colored()
	{
	}

	inline vertex_colored(
			math::float3 const& position,
			math::color	const& color
		) :
		position( position ),
		color	( color )
	{
	}
}; 

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_VERTEX_COLORED_H_INCLUDED