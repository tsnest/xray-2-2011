////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_editor.h"
#include "material.h"
#include "material_instance.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray{
namespace editor{

void material_editor::preview_material_ready( resources::queries_result& data )
{
	m_load_preview_material_query	= NULL;

	R_ASSERT						( data.is_successful() );
	 resources::unmanaged_resource_ptr preview_material	= data[0].get_unmanaged_resource();

	xray::render::editor::renderer* r = m_view_window->editor_renderer();
	r->scene().set_model_material	(	(*m_preview_model)->m_render_model, 
								"", 
								preview_material );
}

void material_editor::query_preview_models( )
{
	m_load_preview_model_query = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &material_editor::preview_models_ready), g_allocator);

	resources::request r[]=
	{
		{"test_object/primitives/cube", resources::static_model_instance_class },
		{"test_object/primitives/sphere", resources::static_model_instance_class }
	};

	resources::query_resources	(
		r,
		boost::bind(&query_result_delegate::callback, m_load_preview_model_query, _1),
		g_allocator
		);
}

void material_editor::preview_models_ready( resources::queries_result& data )
{
	m_load_preview_model_query	= NULL;

	*m_model_cube			= static_cast_resource_ptr<render::static_model_ptr>(data[0].get_unmanaged_resource());
	*m_model_sphere			= static_cast_resource_ptr<render::static_model_ptr>(data[1].get_unmanaged_resource());

	action_preview_model_sphere( );

	if(is_auto_refresh())
		action_refresh_preview_material();
}

void material_editor::set_preview_model( render::static_model_ptr v)
{
	xray::render::editor::renderer* r		= m_view_window->editor_renderer();

	if( (*m_preview_model).c_ptr() )
		r->scene().remove_model				( *m_scene, (*m_preview_model)->m_render_model );

	if (v.c_ptr())
	{
		*m_preview_model						= v;
		r->scene().add_model					( *m_scene, (*m_preview_model)->m_render_model, math::float4x4().identity() );
	}

	
	m_view_window->action_focus_to_origin	( );
}

void material_editor::action_refresh_preview_material( )
{
	if(m_selected_resources->Count!=1 || ((*m_preview_model).c_ptr()==NULL) )
		return;

	configs::lua_config_ptr* lua_config			= NULL;

	{
		material^ mat							= safe_cast<material^>(m_selected_resources[0]);
		if(!mat->loaded || mat->m_is_broken)
			return;
		lua_config								= mat->material_config();
	}

	if(m_load_preview_material_query)
		return;
		//m_load_preview_material_query->m_rejected = true;

	configs::binary_config_ptr binary_config	= (*lua_config)->get_binary_config();
	m_load_preview_material_query				= NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &material_editor::preview_material_ready), g_allocator);

	System::Guid g = System::Guid::NewGuid();
	resources::user_data_variant* ud = NEW(resources::user_data_variant)();
	ud->set(binary_config);

	resources::query_resource(
		unmanaged_string(g.ToString()).c_str(),
		resources::material_class,
		boost::bind(&query_result_delegate::callback, m_load_preview_material_query, _1),
		g_allocator,
		ud
		);
	DELETE(ud);
}

} // namespace editor
} // namespace xray

