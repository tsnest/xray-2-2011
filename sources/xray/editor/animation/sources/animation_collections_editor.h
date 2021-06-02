////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTIONS_EDITOR_H_INCLUDED
#define ANIMATION_COLLECTIONS_EDITOR_H_INCLUDED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace xray::editor_base;

using System::Windows::Forms::MenuStrip;
using System::Windows::Forms::ToolStripMenuItem;
using xray::editor::controls::item_properties_panel_base;

namespace xray
{
	namespace animation_editor
	{
		ref class animation_editor;
		ref class animation_item;
		ref class animation_files_view_panel;
		ref class animation_collections_view_panel;
		ref class animation_collection_item;
		ref class animation_single_item;
		ref class animation_collection_document;
		ref class animation_model;
		
		public ref class animation_collections_editor : public WeifenLuo::WinFormsUI::Docking::DockContent
		{
		public:
			animation_collections_editor			( animation_editor^ ed );
			~animation_collections_editor			( );

			bool			close_query				( );
			IDockContent^	find_dock_content		( String^ persist_string );

			xray::editor::controls::document_base^ on_document_creating	();

			property xray::editor::controls::document_editor_base^	multidocument_base
			{	
				xray::editor::controls::document_editor_base^	get( ) { return m_multidocument_base; }
			}
			property animation_files_view_panel^					animations_view_panel
			{	
				animation_files_view_panel^						get( ) { return m_animations_view_panel; }
			}
			property animation_collections_view_panel^				collections_view_panel
			{	
				animation_collections_view_panel^				get( ) { return m_collections_view_panel; }
			}
			property item_properties_panel_base^					properties_panel
			{
				item_properties_panel_base^						get( ) { return m_instance_properties_panel; }
			}
			property animation_collection_document^					active_document
			{
				animation_collection_document^					get( );
			}
			property animation_model^									active_model
			{
				animation_model^									get( ){ return m_model; }
			}
		
		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->components = gcnew System::ComponentModel::Container();
				this->Size = System::Drawing::Size( 300, 300 );
				this->Text = L"animation_collections_editor";
				this->Padding = System::Windows::Forms::Padding(0);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			}
		#pragma endregion

		public:
			void						tick						( );
			animation_editor^			get_animation_editor		( );
			void						view_on_drop_down_opening	( Object^, EventArgs^ );
			void						show_animations_tree		( Object^, EventArgs^ );
			void						show_collections_tree		( Object^, EventArgs^ );
			void						show_properties_panel		( Object^, EventArgs^ );
			animation_single_item^		get_single					( String^ single_name );
			animation_collection_item^	get_collection				( String^ collection_name );
			animation_collection_item^	new_collection				( String^ collection_name );
			void						rename_collection			( String^ old_name, String^ new_name );
			void						remove_collection			( String^ collection_name );
			animation_collection_item^	find_collection				( String^ collection_name );
			void						show_properties				( animation_item^ item );
			void						show_properties				( animation_item^ item, Boolean is_read_only );
			void						show_properties				( array<animation_item^>^ items );
			void						show_properties				( array<animation_item^>^ items, Boolean is_read_only );
			void						clear_properties			( );
			void						update_properties			( );
			void						scene_created				( xray::render::scene_ptr const& scene );
			void						add_models_to_render		( );
			void						remove_models_from_render	( );

		private:
			bool						predicate_active_scene		( );
			bool						predicate_save_scene		( );
			bool						predicate_save_all			( );
			void						model_loaded				( xray::resources::queries_result& result );
		
		public:
			ToolStripMenuItem^								m_menu_file_save;
			ToolStripMenuItem^								m_menu_edit_undo;
			ToolStripMenuItem^								m_menu_edit_redo;

		private:
			animation_editor^								m_animation_editor;
			animation_files_view_panel^						m_animations_view_panel;
			animation_collections_view_panel^				m_collections_view_panel;
			item_properties_panel_base^						m_instance_properties_panel;
			System::ComponentModel::Container^				components;
			xray::editor::controls::document_editor_base^	m_multidocument_base;

			MenuStrip^										m_menu;

			ToolStripMenuItem^								m_menu_file;
			ToolStripMenuItem^								m_menu_edit;

			ToolStripMenuItem^								m_menu_view;
			ToolStripMenuItem^								m_menu_view_animations_tree;
			ToolStripMenuItem^								m_menu_view_collections_tree;
			ToolStripMenuItem^								m_menu_view_properties_panel;
			
			Dictionary<String^, animation_collection_item^>^	m_collections_items;
			Dictionary<String^, animation_single_item^>^		m_single_items;

			animation_model^									m_model;
			render::scene_ptr*									m_scene;
		};// class animation_collections_editor

		public enum class animation_collections_editor_drag_drop_operation
		{
			tree_view_file = 0,
			tree_view_folder = 1,
		}; //enum class sound_editor_drag_drop_operation
	} //namespace animation_editor
} //namespace xray

#endif // #ifndef ANIMATION_COLLECTIONS_EDITOR_H_INCLUDED

