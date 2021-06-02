////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_editor.h"
#include "material.h"
#include "material_stage.h"
#include "material_effect.h"

namespace xray{
namespace editor{

void material_editor::action_preview_model_cube( )
{
	set_preview_model		( *m_model_cube );
	action_refresh_preview_material( );
}

bool material_editor::is_preview_model_cube( )
{
	return (*m_preview_model) == (*m_model_cube);
}

void material_editor::action_preview_model_sphere( )
{
	set_preview_model		( *m_model_sphere );
	action_refresh_preview_material( );
}

bool material_editor::is_preview_model_sphere( )
{
	return (*m_preview_model) == (*m_model_sphere);
}

void material_editor::action_auto_refresh( )
{
	m_auto_refresh = !m_auto_refresh;
	if(is_auto_refresh())
		action_refresh_preview_material();
}

void material_editor::global_action_auto_refresh( )
{
	m_global_auto_refresh = !m_global_auto_refresh;
	
	m_is_apply_resource_on_property_value_changed = m_global_auto_refresh;
}

bool material_editor::is_auto_refresh( )
{
	return m_auto_refresh;
}

bool material_editor::is_global_auto_refresh( )
{
	return m_global_auto_refresh;
}

void material_editor::action_setup_current_diffuse_texture( )
{
	System::Object^ o = xray::editor_base::resource_chooser::get_shared_context("diffuse_texture");
	System::String^ tname = safe_cast<System::String^>(o);
	
	for each(material^ m in m_selected_resources)
	{
		for each( material_stage^ s in m->m_stages.Values)
			s->m_effect->set_base_texture( tname );
		
		on_resource_changed( m );
	}
}

bool material_editor::is_setup_current_diffuse_texture( )
{
	return (nullptr!=xray::editor_base::resource_chooser::get_shared_context("diffuse_texture"));
}

material_chooser::material_chooser( editor_world& w, material_editor_mode mode )
:m_editor_world	( w ),
m_mode			( mode )
{}

System::String^	material_chooser::id( )
{
	return (m_mode==material_editor_mode::edit_material) ? "material" : "material_instance";
}

bool material_chooser::choose( System::String^ current, System::String^ filter, System::String^% selected, bool read_only )
{
	XRAY_UNREFERENCED_PARAMETERS( filter, read_only );
	material_editor^ ed = gcnew material_editor( m_editor_world, m_mode, true );

	ed->selected_name		= current;

	bool result = (ed->ShowDialog() == System::Windows::Forms::DialogResult::OK);
	if( result )
		selected	= ed->selected_name;

	delete	ed;
	return	result;
}

} // namespace editor
} // namespace xray
