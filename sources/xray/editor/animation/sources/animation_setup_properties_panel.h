////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_SETUP_PROPERTIES_PANEL_H_INCLUDED
#define ANIMATION_SETUP_PROPERTIES_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;
using namespace System::Drawing::Design;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace animation_editor {

	ref class animation_setup_manager;

	public ref class animation_setup_properties_panel: public xray::editor::controls::item_properties_panel_base
	{
		typedef xray::editor::controls::item_properties_panel_base super;
	public:
						animation_setup_properties_panel	(animation_setup_manager^ m);
		virtual	void	show_properties						(Object^ object) override;

	protected:
		~animation_setup_properties_panel()
		{
			if(components)
				delete components;
		}

	private:
		void			on_property_changed					(System::Object^ sender, value_changed_event_args^ e);

	private:
		System::ComponentModel::Container^	components;
		animation_setup_manager^			m_manager;
	}; // class animation_setup_properties_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_SETUP_PROPERTIES_PANEL_H_INCLUDED