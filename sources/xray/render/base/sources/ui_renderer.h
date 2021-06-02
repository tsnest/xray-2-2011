#ifndef UI_RENDERER_H_INCLUDED
#define UI_RENDERER_H_INCLUDED

#include <xray/render/base/ui_renderer.h>

namespace xray {
namespace render {

struct platform;
class base_world;

class ui_command :
	public xray::render::ui::command,
	private boost::noncopyable
{
	logic::vector<ui::vertex>	m_verts;
	int					m_prim_type;
	int					m_point_type;
	u32					m_count;
	platform&			m_platform;
public:
						ui_command			(u32 max_verts, int prim_type, int point_type, platform& p);
						ui_command			( ui::vertex* begin, ui::vertex* end, int prim_type, int point_type, xray::render::platform& p);
	virtual				~ui_command			();
	virtual void		set_shader			(ui::shader& shader);
	virtual void		push_point			(float x, float y, float z, u32 c, float u, float v);

	virtual	void		execute				();
};

class ui_renderer :
	public xray::render::ui::renderer,
	private boost::noncopyable
{
public:
						ui_renderer			( base_world& world, platform& platform );
	virtual				~ui_renderer		();

	virtual	ui::command* create_command		(u32 max_verts, int prim_type, int point_type);
	virtual	ui::command* create_command		(ui::vertex* begin, ui::vertex* end, int prim_type, int point_type);

private:
	base_world&	m_world;
	platform&				m_platform;
}; // class renderer

} // namespace render
} // namespace xray

#endif // #ifndef GAME_RENDERER_H_INCLUDED