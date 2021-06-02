////////////////////////////////////////////////////////////////////////////
//	Created 	: 18.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_EXTENDED_RENDERER_H_INCLUDED
#define XRAY_RENDER_BASE_EXTENDED_RENDERER_H_INCLUDED

namespace xray {
namespace render {
namespace extended {

struct XRAY_NOVTABLE renderer {
	virtual	pcstr	type		( ) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( renderer )
}; // class world

} // namespace extended
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_EXTENDED_RENDERER_H_INCLUDED