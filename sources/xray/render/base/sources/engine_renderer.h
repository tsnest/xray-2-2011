////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ENGINE_RENDERER_H_INCLUDED
#define ENGINE_RENDERER_H_INCLUDED

#include <xray/render/base/engine_renderer.h>

namespace xray {
namespace render {

struct platform;
class base_world;

namespace engine {

class engine_renderer :
	public xray::render::engine::renderer,
	private boost::noncopyable
{
public:
					engine_renderer			( render::base_world& world, platform& platform );
	virtual			~engine_renderer		( );
	virtual	bool	run						( bool wait_for_command );
	virtual	bool	run_editor_commands		( bool wait_for_command );
	virtual	u32		frame_id				( );
	virtual	void	set_view_matrix			( float4x4 const& view_matrix );
	virtual	void	set_projection_matrix	( float4x4 const& projection );
	virtual	void	draw_frame				( );
	virtual	void	purge_orders			( );
	virtual	void	flush_debug_commands	( );
	virtual	void 	test_cooperative_level	( );

	
private:
	render::base_world&		m_world;
	platform&				m_platform;
}; // class renderer

} // namespace engine
} // namespace render
} // namespace xray

#endif // #ifndef ENGINE_RENDERER_H_INCLUDED