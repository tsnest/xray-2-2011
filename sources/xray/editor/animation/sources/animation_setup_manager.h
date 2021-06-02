////////////////////////////////////////////////////////////////////////////
//	Created		: 10.01.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_MANAGER_H_INCLUDED
#define ANIMATION_SETUP_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace WeifenLuo::WinFormsUI::Docking;
using namespace xray::editor::controls;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace animation { 
	namespace mixing { 
		class expression;
		class animation_lexeme;
	} // namespace mixing
} // namespace animation
namespace animation_editor {

	ref class animation_editor;
	ref class animations_list_panel;
	ref class animation_setup_properties_panel;
	ref class animation_model;

	public ref class animation_setup_manager : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
								animation_setup_manager			(animation_editor^ ed);
								~animation_setup_manager		();
								!animation_setup_manager		();
		void					tick							();
		void					clear_resources					();
		bool					close_query						();
		IDockContent^			find_dock_content				(System::String^ persist_string);
		animation_editor^		get_editor						() {return m_editor;};
		document_base^			on_document_creating			();
		void					set_target						(xray::animation::mixing::animation_lexeme& l, u32 const current_time_in_ms);
		bool					predicate_save_scene			();
		bool					model_loaded					();
		void					add_model_to_render				();
		void					remove_model_from_render		();
		void					document_saved					(System::String^ doc_name);
		void					on_scene_created				( render::scene_ptr const& scene );
		void					on_property_changed				(System::Object^ sender, value_changed_event_args^ e);
		void					show_properties					(Object^ obj);

		property document_editor_base^ multidocument_base
		{	
			document_editor_base^	get							() {return m_multidocument_base;}
		}
		property animations_list_panel^	animations_list
		{	
			animations_list_panel^	get							() {return m_animations_list_panel;}
		}
		property animation_setup_properties_panel^	properties_panel
		{	
			animation_setup_properties_panel^	get				() {return m_properties_panel;}
		}

	private:
		void					save_active_document			(System::Object^, System::EventArgs^);
		void					save_all_documents				(System::Object^, System::EventArgs^);
		void					on_resources_loaded				(xray::resources::queries_result& result);
		void					on_load_model_click				(System::Object^, System::EventArgs^);
		void					on_view_menu_item_select		(System::Object^, System::EventArgs^);
		void					show_anim_list_panel			(System::Object^, System::EventArgs^);
		void					show_properties_panel			(System::Object^, System::EventArgs^);
		
	private:
		animation_editor^					m_editor;
		animations_list_panel^				m_animations_list_panel;
		animation_setup_properties_panel^	m_properties_panel;
		document_editor_base^				m_multidocument_base;
		animation_model^						m_model;
		MenuStrip^							m_menu_strip;
		ToolStripMenuItem^					menuToolStripMenuItem;
		ToolStripMenuItem^					loadmodelToolStripMenuItem;
		ToolStripMenuItem^					saveanimationToolStripMenuItem;
		ToolStripMenuItem^					saveallToolStripMenuItem;
		ToolStripMenuItem^					editToolStripMenuItem;
		ToolStripMenuItem^					viewToolStripMenuItem;
		ToolStripMenuItem^					animlistToolStripMenuItem;
		ToolStripMenuItem^					propertiesToolStripMenuItem;
		render::scene_ptr*					m_scene;

	public:
		ToolStripMenuItem^					undoToolStripMenuItem;
		ToolStripMenuItem^					redoToolStripMenuItem;
	}; // class animation_setup_manager
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_MANAGER_H_INCLUDED