////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_ENGINE_RENDERER_H_INCLUDED
#define XRAY_RENDER_FACADE_ENGINE_RENDERER_H_INCLUDED

#include <xray/render/api.h>

namespace xray {
namespace render {
namespace engine {

class world;

class XRAY_RENDER_API renderer : private core::noncopyable {
private:
	friend class render::world;
			renderer	( engine::world& engine_world );

public:
	void	initialize	( );
	u32		frame_id	( );
	
private:
	engine::world&	m_render_engine_world;
}; // class renderer

} // namespace engine
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_ENGINE_RENDERER_H_INCLUDED