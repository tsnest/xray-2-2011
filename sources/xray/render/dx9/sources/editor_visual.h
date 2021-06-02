////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_VISUAL_H_INCLUDED
#define EDITOR_VISUAL_H_INCLUDED

#include <xray/render/base/visual.h>

namespace xray {
namespace render{


struct editor_visual
{
	editor_visual( ){}

	editor_visual(u32 id, xray::render::visual_ptr visual, float4x4 transform, bool selected, bool system_object, bool beditor_chain):
	transform		(transform),
	visual			(visual),
	id				(id),
	selected		(selected),
	system_object	(system_object),
	beditor_chain	(beditor_chain)
	{}

	bool operator < (editor_visual const& other) const 
	{
		return id < other.id;
	}

	bool operator == (u32 other) const 
	{
		return id == other;
	}

	float4x4		transform;
	xray::render::visual_ptr	visual;
	u32				id;
	bool			selected;
	bool			system_object;
	bool			beditor_chain;

}; // class editor_visual

} // namespace render
} // namespace xray


#endif // #ifndef EDITOR_VISUAL_H_INCLUDED