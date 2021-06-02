////////////////////////////////////////////////////////////////////////////
//	Created		: 26.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef CHANNELS_LAYOUT_PANEL_H_INCLUDED
#define CHANNELS_LAYOUT_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace xray::editor::wpf_controls;

namespace xray {
namespace animation_editor {
	public ref class channels_layout_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		channels_layout_panel();
		~channels_layout_panel();

		void	add_curves	(System::Collections::Generic::List<float_curve^>^ curves_list);

	private:
		System::ComponentModel::Container^					components;
		System::Windows::Forms::Integration::ElementHost^	m_host;
		curve_editor::curve_editor_panel^					m_curve_panel;
	}; // ref class channels_layout_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef CHANNELS_LAYOUT_PANEL_H_INCLUDED