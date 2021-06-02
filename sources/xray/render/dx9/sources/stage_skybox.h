////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SKYBOX_H_INCLUDED
#define STAGE_SKYBOX_H_INCLUDED

#include "stage.h"
namespace xray {
namespace render{


class stage_skybox: public stage
{
public:
	stage_skybox(scene_context* context);
	~stage_skybox();

	void execute( );

private:
	void render_sky();
	void render_clouds();

	static void rm_near();
	static void rm_normal();
	static void rm_far();

private:
	ref_shader		m_sh_sky;
	ref_geometry	m_g_skybox;
}; // class stage_skybox

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_SKYBOX_H_INCLUDED