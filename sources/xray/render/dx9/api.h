////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_DX9_API_H_INCLUDED
#define XRAY_RENDER_DX9_API_H_INCLUDED

namespace xray {

namespace render {
	struct world;

	namespace engine {
		struct wrapper;
	} // namespace engine
} // namespace render

namespace render_dx9 {

	XRAY_RENDER_API	render::world*	create_world	( render::engine::wrapper& engine, HWND window_handle );
	XRAY_RENDER_API	void			destroy_world	( render::world*& world );

} // namespace render_dx9

} // namespace xray


#endif // #ifndef XRAY_RENDER_DX9_API_H_INCLUDED