////////////////////////////////////////////////////////////////////////////
//	Created		: 12.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VISUALIZERS_H_INCLUDED
#define PROPERTY_VISUALIZERS_H_INCLUDED

namespace xray {
namespace editor_base {

public interface class property_visualizer
{
	virtual void draw		( math::float4x4 const& world_matrix, xray::render::debug::renderer& r, render::scene_pointer scene ) =0;
};

} // namespace editor_base
} // namespace xray

#endif // #ifndef PROPERTY_VISUALIZERS_H_INCLUDED