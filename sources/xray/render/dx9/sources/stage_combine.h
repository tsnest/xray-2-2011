////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_COMBINE_H_INCLUDED
#define STAGE_COMBINE_H_INCLUDED

#include "stage.h"
namespace xray {
namespace render{


class stage_combine: public stage
{
public:
	stage_combine(scene_context* context);
	~stage_combine();

	void execute( );

private:
	ref_shader		m_sh_combine;
	ref_geometry	m_g_combine;
}; // class stage_combine

	} // namespace render
} // namespace xray


#endif // #ifndef STAGE_COMBINE_H_INCLUDED