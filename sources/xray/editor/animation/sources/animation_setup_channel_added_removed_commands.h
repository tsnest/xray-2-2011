////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_CHANNEL_ADDED_REMOVED_COMMANDS_H_INCLUDED
#define ANIMATION_SETUP_CHANNEL_ADDED_REMOVED_COMMANDS_H_INCLUDED

using namespace System;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace editor_base { ref class command; }
namespace animation_editor {

	ref class animation_setup_document;

	public ref class animation_setup_channel_added_command : xray::editor_base::command
	{
	public:
						animation_setup_channel_added_command	(animation_setup_document^ d, setup_panel_event_args^ e);
		virtual			~animation_setup_channel_added_command	();

		virtual bool	commit									() override;
		virtual void	rollback								() override;

	private:
		animation_setup_document^	m_document;
		String^						m_channel_name;
		Guid						m_first_partition_id;
	}; // ref class animation_setup_channel_added_command

	public ref class animation_setup_channel_removed_command : xray::editor_base::command
	{
	public:
						animation_setup_channel_removed_command	(animation_setup_document^ d, setup_panel_event_args^ e);
		virtual			~animation_setup_channel_removed_command();

		virtual bool	commit									() override;
		virtual void	rollback								() override;

	private:
		animation_setup_document^	m_document;
		pstr						m_cfg_string;
		String^						m_channel_name;
	}; // ref class animation_setup_channel_removed_command

	public ref class animation_setup_paste_channel_command : xray::editor_base::command
	{
	public:
						animation_setup_paste_channel_command	(animation_setup_document^ d, pcstr cfg_string, u32 max_frames);
		virtual			~animation_setup_paste_channel_command	();

		virtual bool	commit									() override;
		virtual void	rollback								() override;

	private:
		animation_setup_document^	m_document;
		pstr						m_cfg_string;
		String^						m_channel_name;
		u32							m_max_frames;
	}; // ref class animation_setup_paste_channel_command
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_CHANNEL_ADDED_REMOVED_COMMANDS_H_INCLUDED