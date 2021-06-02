////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef INSTANCE_PROPERTIES_PANEL_H_INCLUDED
#define INSTANCE_PROPERTIES_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;
using namespace System::Drawing::Design;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace sound_editor {

	ref class sound_editor;

	public ref class instance_properties_panel : public item_properties_panel_base
	{
		typedef item_properties_panel_base super;
	public:
						instance_properties_panel		(sound_editor^ ed);
		virtual	void	show_properties					(Object^ object) override;

	protected:
		~instance_properties_panel()
		{
			if(components)
				delete components;
		}

	private:
		void			on_node_property_changed		(System::Object^ sender, value_changed_event_args^ e);

	private:
		System::ComponentModel::Container^	components;
		sound_editor^						m_owner;
	}; // ref class instance_properties_panel
} // namespace sound_editor
} // namespace xray
#endif // #ifndef INSTANCE_PROPERTIES_PANEL_H_INCLUDED