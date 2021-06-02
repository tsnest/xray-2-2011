////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_ACCUMULATE_DISTORTION_H_INCLUDED
#define STAGE_ACCUMULATE_DISTORTION_H_INCLUDED

#include "stage.h"
#include "render_model.h"

namespace xray {
namespace render {

class stage_accumulate_distortion: public stage
{
public:
					stage_accumulate_distortion		( renderer_context* context );
					~stage_accumulate_distortion	( );
	virtual void	execute							( );
	
private:
	render_surface_instances		m_dynamic_visuals;
}; // class stage_accumulate_distortion

} // namespace render
} // namespace xray
#endif // #ifndef STAGE_ACCUMULATE_DISTORTION_H_INCLUDED