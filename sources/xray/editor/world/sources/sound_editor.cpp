////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_editor.h"
#include "sound_files_view_panel.h"
#include "sound_items_view_panel.h"
#include "raw_file_property_struct.h"
#include "editor_world.h"
#include "sound_file_struct.h"
#include "sound_item_struct.h"
#include "sound_object_cook.h"
#include "sound_item_editor_panel.h"


using namespace WeifenLuo::WinFormsUI::Docking;
using namespace System::Windows::Forms;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Collections::Generic;

using namespace xray::editor;

using xray::editor::sound_editor;
// using xray::resources::ogg_sound_cook_wrapper;
// using xray::resources::ogg_sound_cook;
// using xray::resources::ogg_cook;
// using xray::resources::ogg_converter;
// using xray::resources::ogg_options_converter;
using xray::resources::resources_manager;

namespace xray
{
	namespace editor
	{
		sound_editor::sound_editor(xray::editor::editor_world& world):m_world(world)
		{
// 			static ogg_sound_cook_wrapper			ogg_cook_wrapper_obj;
// 			register_cook			(&ogg_cook_wrapper_obj);
// 
// 			static ogg_cook					ogg_cook_obj;
// 			register_cook			(&ogg_cook_obj);
// 				
// 			static ogg_sound_cook			ogg_cook_converter_obj;
// 			register_cook			(&ogg_cook_converter_obj);
// 
// 			static ogg_options_converter	ogg_options_converter_obj;
// 			register_cook			(&ogg_options_converter_obj);
// 
// 			static ogg_converter			ogg_converter_obj;
// 			register_cook			(&ogg_converter_obj);

			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			sound_resources_path			= "resources/sounds/";
			absolute_sound_resources_path	= Path::GetFullPath(gcnew String(m_world.engine().get_resource_path())+"/sounds/");
			m_sound_files			= gcnew Dictionary<String^, sound_file_struct^>();
			m_sound_items			= gcnew Dictionary<String^, sound_item_struct^>();

			dock_panels();
		}

		sound_editor::~sound_editor()
		{
			if (components)
			{
				delete components;
			}
		}

		//dock panels to sound editor main form
		void			sound_editor::dock_panels				()
		{
			//Create panels
			m_sound_files_panel				= gcnew sound_files_view_panel();
			m_sound_items_panel				= gcnew sound_items_view_panel();
			m_sound_item_editor_panel		= gcnew sound_item_editor_panel();

			//fill events
			m_sound_items_panel->sound_item_selected			+= gcnew EventHandler<sound_item_selection_event_args^>(this, &sound_editor::sound_item_selected);

			//Load panels settings
			controls::serializer::deserialize_dock_panel_root(this, m_main_dock_panel, "sound_editor",
				gcnew DeserializeDockContent(this, &sound_editor::reload_content));
			
			//Show panels
			m_sound_files_panel				-> Show(m_main_dock_panel);	
			m_sound_items_panel				-> Show(m_main_dock_panel);
			m_sound_item_editor_panel		-> Show(m_main_dock_panel);

		}

		//Function handles when user try to close sound editor
		void			sound_editor::sound_editor_FormClosing	(System::Object^  , System::Windows::Forms::FormClosingEventArgs^  e)
		{
			if(e->CloseReason == System::Windows::Forms::CloseReason::UserClosing)
			{
				//manual hide this form
				this->Hide();
				//cancel closing the form
				e->Cancel = true;
			}

			//Save panel settings
			controls::serializer::serialize_dock_panel_root(this, m_main_dock_panel, "sound_editor");
		}

		//return's panel to set it settings
		IDockContent^	sound_editor::reload_content			(System::String^ persist_string)
		{
			if (persist_string == "xray.editor.sound_files_view_panel")
				return			m_sound_files_panel;

			if (persist_string == "xray.editor.sound_items_view_panel")
				return			m_sound_items_panel;

			if (persist_string == "xray.editor.sound_item_editor_panel")
				return			m_sound_item_editor_panel;

			return				(nullptr);
		}

		//close form and save its settings
		void			sound_editor::manual_close				()
		{
			//Save panel settings
			controls::serializer::serialize_dock_panel_root(this, m_main_dock_panel, "sound_editor");
			this->Close();
		}

		//View properties in property panel
		void			sound_editor::show_properties			(Object^ object)
		{
			m_sound_files_panel->show_properties(object);
		}

		void			sound_editor::save_file_menu_item_click	(System::Object^ , System::EventArgs^ )
		{
			m_sound_files_panel->selected_struct->save();
		}

		void			sound_editor::sound_item_selected		(Object^ , sound_item_selection_event_args^ e)
		{
			m_sound_item_editor_panel->selected_item = e->m_struct;
		}
	}
}