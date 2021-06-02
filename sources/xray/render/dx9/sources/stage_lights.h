////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_LIGHTS_H_INCLUDED
#define STAGE_LIGHTS_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render{


// Forward declaretions
class light;

class stage_lights: public stage
{
public:
	stage_lights			( scene_context* context);
	~stage_lights			();

	void execute			( );

private:
	void accum_point		( light* l);
	void reset_light_marker	( bool bResetStencil);
	void increment_light_marker();
	void draw_sphere		( light* l);

private:
	ref_shader					m_sh_accum_point;

	IDirect3DVertexBuffer9*		m_accum_point_vb;
	IDirect3DIndexBuffer9*		m_accum_point_ib;

	ref_geometry	m_g_accum_point;
}; // class stage_lights

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_LIGHTS_H_INCLUDED