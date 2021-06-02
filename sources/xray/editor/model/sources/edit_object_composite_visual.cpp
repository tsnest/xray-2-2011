////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_object_composite_visual.h"
#include "property_grid_panel.h"
#include "model_editor.h"
#include "ide.h"

namespace xray{
namespace model_editor{

edit_object_composite_visual::edit_object_composite_visual( model_editor^ parent )
:super( parent )
{
	m_type_name				= "composite";
	m_model_config			= NEW(configs::lua_config_ptr)();
	m_object_contents_panel = gcnew property_grid_panel;
}

edit_object_composite_visual::~edit_object_composite_visual( )
{
	delete		m_object_contents_panel;
	DELETE		( m_model_config );
}

void edit_object_composite_visual::tick( )
{
	super::tick();
}

IDockContent^ edit_object_composite_visual::find_dock_content( System::String^ persist_string )
{
	if(persist_string=="xray.model_editor.property_grid_panel")
		return m_object_contents_panel;
	else
		return nullptr;
}
void edit_object_composite_visual::refresh_surfaces_panel( )
{
	m_object_contents_panel->set_property_container( get_contents_property_container() );
}

void edit_object_composite_visual::clear_resources( )
{
	super::clear_resources	( );
	for each( composite_visual_item^ item in m_contents )
		delete item;

	m_contents.Clear();
}

void edit_object_composite_visual::set_default_layout( )
{
	m_object_contents_panel->Show	( m_model_editor->get_ide()->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft );
}

bool edit_object_composite_visual::complete_loaded( )
{
	return true;
}

void edit_object_composite_visual::reset_selection( )
{
}

math::aabb edit_object_composite_visual::focused_bbox( )
{
	math::aabb result = math::create_identity_aabb();

	for each( composite_visual_item^ item in m_contents )
	{
		if(item->get_selected())
			return item->get_aabb();
		else
			result.modify(item->get_aabb());
	}
	return result;
}
}
}
