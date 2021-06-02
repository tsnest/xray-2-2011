////////////////////////////////////////////////////////////////////////////
//	Created		: 20.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_inspector_tab.h"
#include "object_composite.h"
#include "tool_base.h"
#include "project_items.h"

namespace xray {
namespace editor {

void object_inspector_tab::in_constructor()
{
	Width						= 100;
	Height						= 100;
	m_property_grid_host		= gcnew host( );
	m_property_grid_host->refresh_property_changed	+= gcnew System::EventHandler( this, &object_inspector_tab::on_refresh_value_changed );
	m_property_grid_host->Width	= 100;
	m_property_grid_host->Height= 100;
	m_property_grid_host->Dock	= System::Windows::Forms::DockStyle::Fill;

	SuspendLayout			( );
	Controls->Add			( m_property_grid_host );
	ResumeLayout			( false );
}

void object_inspector_tab::show_properties( project_items_list^ items )
{
	m_last_show_properties_items				= items;

	if(!items) 
	{
		m_property_grid_host->selected_object	= nullptr;
		return;
	}

	cli::array<System::Object^>^ properties		= gcnew cli::array<System::Object^> (items->Count);
	int idx										= 0;
	
	for each( project_item_base^ itm in items )
	{
		wpf_controls::property_container^ container	= nullptr;
		object_base^ o							= itm->get_object();

		if(o)
		{// object
			if( o->get_parent_object() ) 
			{
				object_composite^ oc			= safe_cast<object_composite^>( o->get_parent_object() ); // must be compound!
				container						= oc->get_object_property_container( o );
			}else
				container						= o->get_property_container( );
		}else
		{//folder ?
			project_item_folder^ folder = dynamic_cast<project_item_folder^>(itm);
			if(folder)
			{
				container						= folder->get_property_container( );
			}
		}
		properties[ idx++ ]						= container;
	}

	m_property_grid_host->selected_objects		= properties;
}

void object_inspector_tab::show_properties ( Object^ object )
{
	m_property_grid_host->selected_object		= object;
}

void object_inspector_tab::on_refresh_value_changed	( System::Object^ sender, System::EventArgs^ args )
{
	XRAY_UNREFERENCED_PARAMETERS				( sender, args );
	show_properties								( m_last_show_properties_items );
}

// requery property values
void object_inspector_tab::update( )
{
	m_property_grid_host->update();
}
// reset objects in grid
void object_inspector_tab::reset( )
{
	m_property_grid_host->reset();
}

void object_inspector_tab::remove_object_from_selection( Object^ obj )
{
	m_property_grid_host->property_grid->remove_object_from_selection( obj );
}


} // namespace editor
} // namespace xray