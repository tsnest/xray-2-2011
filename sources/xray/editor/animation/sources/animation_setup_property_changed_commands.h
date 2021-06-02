////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_PROPERTY_CHANGED_H_INCLUDED
#define ANIMATION_SETUP_PROPERTY_CHANGED_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace xray::editor::wpf_controls::animation_setup;

namespace xray {
namespace editor_base { ref class command; }
namespace animation_editor {

	ref class animation_setup_document;

	public ref class animation_setup_item_property_changed_command : xray::editor_base::command
	{
	public:
						animation_setup_item_property_changed_command	(animation_setup_document^ d, setup_panel_event_args^ e);
		virtual			~animation_setup_item_property_changed_command	();

		virtual bool	commit												() override;
		virtual void	rollback											() override;

	private:
		animation_setup_document^	m_document;
		String^						m_channel_name;
		List<Guid>^					m_item_ids;
		List<String^>^				m_property_names;
		List<Object^>^				m_old_values;
		List<Object^>^				m_new_values;
	}; // ref class animation_setup_item_property_changed_command

	public ref class animation_setup_property_changed_command: xray::editor_base::command
	{
		typedef xray::editor::wpf_controls::property_container wpf_property_container;
	public:
						animation_setup_property_changed_command	(animation_setup_document^ d, System::String^ l, Object^ new_val, Object^ old_val);
		virtual			~animation_setup_property_changed_command	() {};

		virtual bool	commit										() override;
		virtual void	rollback									() override;

	private:
		animation_setup_document^	m_document;
		System::String^ 			m_changed_property_name;
		System::Object^ 			m_old_value;
		System::Object^ 			m_new_value;
	}; // ref class animation_setup_property_changed_command
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_PROPERTY_CHANGED_H_INCLUDED