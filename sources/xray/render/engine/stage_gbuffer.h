////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_GBUFFER_H_INCLUDED
#define STAGE_GBUFFER_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render_dx10 {

class stage_gbuffer: public stage
{
public:
			stage_gbuffer	( scene_context* context);
			~stage_gbuffer	();

	// Executes Geometry buffer stage.
	// rt_position, rt_normal, rt_color: G buffer render targets
	// zb: Z buffer
	// Notes: The function clears all render targets and Z buffer.
	void	execute			( );

private:
	//just to minimize memory allocations
	//vector will adapt theirs size after some time
	vector<render_visual*>	m_visuals;
	render_queue<key_rq, queue_item, key_rq_sort_desc>	m_static_rq;
	ref_state				m_state;
}; // class stage_gbuffer

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_GBUFFER_H_INCLUDED