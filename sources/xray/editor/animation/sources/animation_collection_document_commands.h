////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_COLLECTION_DOCUMENT_COMMANDS_H_INCLUDED
#define ANIMATION_COLLECTION_DOCUMENT_COMMANDS_H_INCLUDED

#include "animation_item_type.h"
#include "animation_item.h"

using namespace System;
using namespace System::Collections::Generic;
using xray::editor::controls::tree_node;
using xray::editor::wpf_controls::hierarchy_item_id;

namespace xray
{
	namespace animation_editor
	{
		ref class animation_collection_document;

		public ref class animation_collection_document_add_items_command: public xray::editor_base::command
		{
		public:
			animation_collection_document_add_items_command		( animation_collection_document^ d, hierarchy_item_id^ destination_index, List<String^>^ items_paths, animation_item_type item_type );
			~animation_collection_document_add_items_command	( );

			virtual bool	commit				( ) override;
			virtual void	rollback			( ) override;

		private:
			animation_collection_document^	m_document;
			hierarchy_item_id^				m_destination_index;
			List<String^>^					m_new_items_paths;
			animation_item_type				m_items_type;

		}; // ref class animation_collection_document_add_items_command

		public ref class animation_collection_document_remove_items_command: public xray::editor_base::command
		{
		public:
			animation_collection_document_remove_items_command	( animation_collection_document^ d, List<tree_node^>^ items_list );
			~animation_collection_document_remove_items_command	( );

			virtual bool	commit				( ) override;
			virtual void	rollback			( ) override;

		private:
			value class stored_item_data;

			animation_collection_document^		m_document;
			List<stored_item_data>^				m_saved_data;

		private:
			value class stored_item_data
			{
			public:
				stored_item_data ( hierarchy_item_id^ index, String^ name, animation_item_type type )
				{
					m_index = index;
					m_name	= name;
					m_type	= type;
				}
				stored_item_data ( hierarchy_item_id^ index, String^ name, animation_item_type type, animation_item^ item )
				{
					m_index = index;
					m_name	= name;
					m_type	= type;
					m_item	= item;
				}

			public:
				hierarchy_item_id^				m_index;
				String^							m_name;
				animation_item_type				m_type;
				animation_item^					m_item;
			}; // value class stored_item_data

		}; // ref class animation_collection_document_remove_items_command

		public ref class animation_collection_document_reorganize_command: public xray::editor_base::command
		{
		public:
			animation_collection_document_reorganize_command	( animation_collection_document^ d, hierarchy_item_id^ destination_index, List<tree_node^>^ nodes_list );
			~animation_collection_document_reorganize_command	( );

			virtual bool	commit								( ) override;
			virtual void	rollback							( ) override;

		private:
			animation_collection_document^		m_document;
			hierarchy_item_id^					m_destination_id;
			array<hierarchy_item_id^>^			m_stored_ids;

		}; // ref class animation_collection_document_change_objects_property_command

		public ref class animation_collection_document_change_objects_property_command: public xray::editor_base::command
		{
			typedef List<String^> paths_list;
		public:
			animation_collection_document_change_objects_property_command	( animation_collection_document^ d, hierarchy_item_id^ destination_id, String^ prop_name, Object^ old_val, Object^ new_val );
			~animation_collection_document_change_objects_property_command	( );

			virtual bool	commit								( ) override;
			virtual void	rollback							( ) override;

		private:
			animation_collection_document^		m_document;
			hierarchy_item_id^					m_destination_id;
			String^								m_property_name;
			Object^								m_old_val;
			Object^								m_new_val;

		}; // ref class animation_collection_document_change_objects_property_command

		public ref class animation_collection_document_create_collection_command: public xray::editor_base::command
		{
		public:
			animation_collection_document_create_collection_command		( animation_collection_document^ document, hierarchy_item_id^ destination_id );
			~animation_collection_document_create_collection_command	( );

			virtual bool	commit						( ) override;
			virtual void	rollback					( ) override;

		private:
			animation_collection_document^		m_document;
			hierarchy_item_id^					m_destination_id;

		}; // class sound_collection_create_collection_command

	} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_COLLECTION_DOCUMENT_COMMANDS_H_INCLUDED