////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTION_DOCUMENT_H_INCLUDED
#define ANIMATION_COLLECTION_DOCUMENT_H_INCLUDED

#include "animation_item_type.h"
#include <xray/animation/animation_callback.h>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using xray::editor::controls::tree_view;
using xray::editor::controls::tree_node;
using xray::editor_base::command_engine;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;
using xray::editor::wpf_controls::hierarchy_item_id;

namespace xray 
{
	namespace animation_editor 
	{
		ref class animation_collections_editor;
		ref class animation_collection_item;
		ref class animation_item;

		public ref class animation_collection_document : public xray::editor::controls::document_base
		{
			typedef System::Windows::Forms::ContextMenuStrip ContextMenuStrip;
			typedef xray::editor::controls::document_base super;
			typedef List<animation_item^> items_list;
		public:
									animation_collection_document	( animation_collections_editor^ editor );
									animation_collection_document	( animation_collections_editor^ editor, animation_collection_item^ collection );
									~animation_collection_document	( );
		private:
					void				in_constructor					( );

		public:
			virtual	void				save							( ) override;
			virtual	void				load							( ) override;
			virtual	void				undo							( ) override;
			virtual	void				redo							( ) override;
			virtual	void				copy							( bool del ) override;
			virtual	void				paste							( ) override;
			virtual	void				select_all						( ) override;
			virtual	void				del								( ) override;
			animation_collections_editor^	get_editor					( ) { return m_editor; };
					void				insert_items_into_collection	( List<String^>^ items_paths, animation_item_type type );
					void				insert_items_into_collection	( List<String^>^ items_paths, hierarchy_item_id^ destination, animation_item_type type );
					void				reorganize_nodes				( List<tree_node^>^ nodel_list, hierarchy_item_id^ dest_index );
					tree_view^			get_tree_view					( ) { return m_tree_view; };
		animation_collection_item^		collection						( ) { return m_collection; };
					tree_node^			collection_node					( ) { return m_collection_node; };
					Boolean				try_close						( );

		internal:
					void				insert_item_into_collection		( hierarchy_item_id^ destination, animation_item^ item );
					void				remove_item_from_collection		( hierarchy_item_id^ id );

					void				rename_to						( String^ new_name, String^ old_name );

					void				update_properties				( );
					void				update_tree_names				( );
					void				update_tree						( tree_node^ node, animation_collection_item^ item );
					void				tick							( );
					tree_node^			get_node_by_id					( hierarchy_item_id^ id );
					tree_node^			get_parent_node_by_id			( hierarchy_item_id^ id );
					hierarchy_item_id^	get_id_from_node				( tree_node^ node );
					hierarchy_item_id^	get_id_for_new_child			( tree_node^ node );
		protected:
					virtual void		on_form_closing					( Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e ) override;

		private:
					void				property_changed				( Object^, value_changed_event_args^ e );
					void				resources_loaded				( Object^, EventArgs^ );
					void				fill_tree_view					( tree_node^ root, items_list^ itms );
					void				document_activated				( Object^, EventArgs^ );
					void				document_mouse_down				( Object^, MouseEventArgs^ e );
					void				document_mouse_move				( Object^, MouseEventArgs^ e );
					void				drag_over						( Object^, DragEventArgs^ e );
					void				drag_drop						( Object^, DragEventArgs^ e );
					void				selection_changed				( Object^, xray::editor::controls::tree_view_selection_event_args^ );
					void				menu_opened						( Object^, EventArgs^ );
					void				remove_click					( Object^, EventArgs^ );
					void				add_collection_click			( Object^, EventArgs^ );
					tree_node^			get_node_by_tag					( TreeNodeCollection^ root, animation_item^ tag );
					void				animation_ended					( animation::skeleton_animation_ptr const& anim, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data );
					Boolean				query_close						( );

		private:
			System::ComponentModel::Container^	components;
			animation_collections_editor^		m_editor;
			command_engine^						m_command_engine;
			animation_collection_item^			m_collection;
			String^								m_name;
			SaveFileDialog^						m_save_file_dialog;

			ContextMenuStrip^					m_context_menu;
			ToolStripMenuItem^					m_context_menu_remove;
			ToolStripMenuItem^					m_context_menu_add_collection;

			tree_view^							m_tree_view;
			tree_node^							m_collection_node;
			tree_node^							m_drag_node;
			ImageList^							m_tree_view_image_list;

			animation_item^						m_selected_animation_item;
			tree_node^							m_selected_node;

			xray::timing::timer*				m_timer;
			u32									m_current_time;

			bool								m_first_save;
			tree_node^							m_loading_node;
			bool								m_is_in_try_close;
	
		}; //animation_editor 
	} // namespace 
} // namespace xray

#endif // #ifndef ANIMATION_COLLECTION_DOCUMENT_H_INCLUDED#pragma once
