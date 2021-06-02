////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_PROPERTIES_PANEL_H_INCLUDED
#define ANIMATION_VIEWER_PROPERTIES_PANEL_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace xray::editor::controls;
using namespace System::Drawing::Design;

using xray::editor::wpf_controls::property_grid::value_changed_event_args;

namespace xray {
namespace animation_editor {
	ref class viewer_editor;

	public ref class animation_viewer_properties_panel : public xray::editor::controls::item_properties_panel_base
	{
		typedef xray::editor::controls::item_properties_panel_base super;
	public:
						animation_viewer_properties_panel	(viewer_editor^ ed);
		virtual	void	show_properties						(Object^ object) override;

	protected:
		~animation_viewer_properties_panel()
		{
			if(components)
				delete components;
		}

	private:
		void			on_node_property_changed	(System::Object^ sender, value_changed_event_args^ e);

	private:
		System::ComponentModel::Container^	components;
		viewer_editor^						m_owner;
	}; // ref class animation_viewer_properties_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_PROPERTIES_PANEL_H_INCLUDED