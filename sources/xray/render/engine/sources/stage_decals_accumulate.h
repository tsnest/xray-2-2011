////////////////////////////////////////////////////////////////////////////
//	Created		: 05.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_DECALS_ACCUMULATE_H_INCLUDED
#define STAGE_DECALS_ACCUMULATE_H_INCLUDED

#include "stage.h"
#include "render_model.h"
#include <xray/render/core/shader_constant_host.h>

namespace xray {
namespace render {

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

struct decal_instance;

class stage_decals_accumulate: public stage
{
public:
					stage_decals_accumulate		(renderer_context* context);
					~stage_decals_accumulate	();
	virtual void	execute						();
private:
			bool	is_effects_ready			() const;
	ref_effect									m_opaque_geometry_mask_effect;

}; // class stage_decals_accumulate

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_DECALS_ACCUMULATE_H_INCLUDED
