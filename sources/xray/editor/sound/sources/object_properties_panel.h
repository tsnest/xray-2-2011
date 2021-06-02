////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef OBJECT_PROPERTIES_PANEL_H_INCLUDED
#define OBJECT_PROPERTIES_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;
using namespace System::Drawing::Design;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace sound_editor {

	ref class sound_editor;

	public ref class object_properties_panel : public item_properties_panel_base
	{
		typedef item_properties_panel_base super;
	public:
						object_properties_panel		(sound_editor^ ed);
		virtual	void	show_properties				(Object^ object) override;

	protected:
		~object_properties_panel()
		{
			if(components)
				delete components;
		}

	private:
		void			on_node_property_changed	(System::Object^ sender, value_changed_event_args^ e);

	private:
		System::ComponentModel::Container^	components;
		sound_editor^						m_owner;
	}; // ref class object_properties_panel
} // namespace sound_editor
} // namespace xray
#endif // #ifndef OBJECT_PROPERTIES_PANEL_H_INCLUDED