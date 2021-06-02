////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_ITEM_ADDED_REMOVED_COMMANDS_H_INCLUDED
#define ANIMATION_SETUP_ITEM_ADDED_REMOVED_COMMANDS_H_INCLUDED

using namespace System;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace editor_base { ref class command; }
namespace animation_editor {

	ref class animation_setup_document;

	public ref class animation_setup_item_added_command : xray::editor_base::command
	{
	public:
						animation_setup_item_added_command	(animation_setup_document^ d, setup_panel_event_args^ e);
		virtual			~animation_setup_item_added_command	();

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		animation_setup_document^	m_document;
		String^						m_channel_name;
		Guid						m_item_id;
		Guid						m_partition_to_split_id;
		Single						m_position;
	}; // ref class animation_setup_item_added_command

	public ref class animation_setup_item_removed_command : xray::editor_base::command
	{
	public:
						animation_setup_item_removed_command	(animation_setup_document^ d, setup_panel_event_args^ e);
		virtual			~animation_setup_item_removed_command	();

		virtual bool	commit									() override;
		virtual void	rollback								() override;

	private:
		animation_setup_document^			m_document;
		String^								m_channel_name;
		Guid								m_item_id;
		Guid								m_partition_to_split_id;
		Single								m_position;
		Single								m_length;
		animation_channel_partition_type	m_partition_type;
		u32									m_partition_insert_id;
	}; // ref class animation_setup_item_removed_command
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_ITEM_ADDED_REMOVED_COMMANDS_H_INCLUDED