////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLEND_MODE_H_INCLUDED
#define BLEND_MODE_H_INCLUDED

namespace xray {
namespace render {

enum enum_blend_mode
{
	blend_mode_opaque = 0,
	blend_mode_translucent,
	blend_mode_additive,
	blend_mode_modulate,
	blend_mode_subtractive,
	
}; // enum enum_blend_mode
	
} // namespace render
} // namespace xray

#endif // #ifndef BLEND_MODE_H_INCLUDED

