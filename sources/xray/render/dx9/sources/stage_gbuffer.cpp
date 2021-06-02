////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_gbuffer.h"
#include "model_manager.h"
#include "visual.h"
#include <xray/render/common/sources/terrain.h>

namespace xray {
namespace render{

stage_gbuffer::stage_gbuffer(scene_context* context): stage(context)
{
}

stage_gbuffer::~stage_gbuffer()
{
}

void stage_gbuffer::execute( )
{
	PIX_EVENT(stage_gbuffer);

	set_rt(	m_context->m_rt_position, m_context->m_rt_normal, m_context->m_rt_color, m_context->m_base_zb);
	backend::get_ref().clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

//////////////////////////////////////////////////////////////////////////
	if( model_manager::get_ref().get_draw_editor())
	{
		model_manager::Editor_Visuals& dynamic_visuals = model_manager::get_ref().get_editor_visuals();

		model_manager::Editor_Visuals::iterator	it_d  = dynamic_visuals.begin(),
			end_d = dynamic_visuals.end();

		backend::get_ref().set_cull_mode(D3DCULL_CCW);
		backend::get_ref().set_color_write_enable();

		for (; it_d!=end_d; ++it_d)
		{
			render_visual* visual = static_cast<render_visual*>(&(*(it_d->visual)));
			ASSERT(visual);
			if( !it_d->beditor_chain || it_d->system_object )
				continue;

			m_context->set_w( it_d->transform );
			visual->render();
		}

////// Rendering terrain cells ///////////////////////////////////////////

		terrain::cells const& terrain_cells = terrain::get_ref().get_editor_cells();

		terrain::cells::const_iterator it_tr = terrain_cells.begin();
		terrain::cells::const_iterator en_tr = terrain_cells.end();

		m_context->set_w( math::float4x4().identity() );

		for( ; it_tr != en_tr; ++it_tr)
			it_tr->visual->render();

	}

	if( model_manager::get_ref().get_draw_game())
	{
		model_manager::Editor_Visuals& dynamic_visuals = model_manager::get_ref().get_editor_visuals();

		model_manager::Editor_Visuals::iterator	it_d  = dynamic_visuals.begin(),
			end_d = dynamic_visuals.end();

		backend::get_ref().set_cull_mode(D3DCULL_CCW);
		backend::get_ref().set_color_write_enable();

		for (; it_d!=end_d; ++it_d)
		{
			render_visual* visual = static_cast<render_visual*>(&(*(it_d->visual)));
			ASSERT(visual);
			if( it_d->beditor_chain || it_d->system_object )
				continue;

			m_context->set_w( it_d->transform );
			visual->render();
		}

		////// Rendering terrain cells ///////////////////////////////////////////
		terrain::cells const& terrain_cells = terrain::get_ref().get_game_cells();

		terrain::cells::const_iterator it_tr = terrain_cells.begin();
		terrain::cells::const_iterator en_tr = terrain_cells.end();

		m_context->set_w( math::float4x4().identity() );

		for( ; it_tr != en_tr; ++it_tr)
			it_tr->visual->render();



		model_manager::get_ref().select_visuals(m_context->get_view_pos(), m_context->get_vp()/*m_context->m_full_xform*/, m_visuals);
		vector<render_visual*>::iterator	it  = m_visuals.begin(),
											end = m_visuals.end();

		for (; it!=end; ++it)
		{
			render_visual* visual = *it;
			//ASSERT(visual);
			if( !visual )
				continue;

			if (!visual->m_shader)
				continue;

			key_rq key;
			ref_pass pass = visual->m_shader->m_sh_techniques[0]->m_passes[0];
			queue_item item = {pass, visual, 0};

			key.priority = pass->get_priority();
			m_static_rq.add_dip(key, item);
		}

		//CHK_DX(HW.pDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE,FALSE));
		backend::get_ref().set_stencil(TRUE,D3DCMP_ALWAYS,0x01,0xff,0xff,D3DSTENCILOP_KEEP,D3DSTENCILOP_REPLACE,D3DSTENCILOP_KEEP);
		backend::get_ref().set_cull_mode(D3DCULL_CCW);
		backend::get_ref().set_color_write_enable();

		m_static_rq.render(true);
	}

	m_visuals.clear();
}

} // namespace render
} // namespace xray

