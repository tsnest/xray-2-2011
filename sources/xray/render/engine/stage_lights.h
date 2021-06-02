////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_LIGHTS_H_INCLUDED
#define STAGE_LIGHTS_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render_dx10 {


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
	ref_effect					m_sh_accum_point;

	ref_buffer		m_accum_point_vb;
	ref_buffer		m_accum_point_ib;

// 	ID3DVertexBuffer*		m_accum_point_vb;
// 	ID3DIndexBuffer*		m_accum_point_ib;

	ref_geometry	m_g_accum_point;

	constant_host*	m_c_ldynamic_color;
	constant_host*	m_c_ldynamic_pos;
	constant_host*	m_c_texgen;

}; // class stage_lights

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_LIGHTS_H_INCLUDED