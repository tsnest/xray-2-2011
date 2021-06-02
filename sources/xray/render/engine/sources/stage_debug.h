////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_DEBUG_H_INCLUDED
#define STAGE_DEBUG_H_INCLUDED

#include "stage.h"
#include "render_model.h"

namespace xray {
namespace render {

class stage_debug: public stage
{
public:
						stage_debug			( renderer_context* context );
						~stage_debug		( );
	virtual void		execute				( );
	render_surface_instances		m_dynamic_visuals;
}; // class stage_debug

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_DEBUG_H_INCLUDED