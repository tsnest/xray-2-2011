////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_COMBINE_H_INCLUDED
#define STAGE_COMBINE_H_INCLUDED

#include "stage.h"
namespace xray {
namespace render_dx10 {


class stage_combine: public stage
{
public:
	stage_combine(scene_context* context);
	~stage_combine();

	void execute( );

private:
	ref_effect		m_sh_combine;
	ref_geometry	m_g_combine;
	ref_buffer		m_vb;

	constant_host*	 m_L_ambient;
	constant_host*	 m_L_sun_color;
	constant_host*	 m_Ldynamic_dir;
	constant_host*	 m_env_color;
	constant_host*	 m_fog_color;	
	constant_host*	 m_m_v2w;

}; // class stage_combine

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_COMBINE_H_INCLUDED