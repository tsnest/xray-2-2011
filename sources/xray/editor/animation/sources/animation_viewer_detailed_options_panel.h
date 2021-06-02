////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_VIEWER_DETAILED_OPTIONS_PANEL_H_INCLUDED
#define XRAY_ANIMATION_VIEWER_DETAILED_OPTIONS_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace animation_editor {
	ref class viewer_editor;
	
	public ref class animation_viewer_detailed_options_panel : public System::Windows::Forms::Form
	{
	public:
				animation_viewer_detailed_options_panel		(viewer_editor^ ed);
		void	show										();

	protected:
				~animation_viewer_detailed_options_panel	();

	private:
		void	on_checkbox1_checked_changed				(System::Object^, System::EventArgs^);
		void	on_ok_clicked								(System::Object^, System::EventArgs^);
		void	on_numeric1_value_changed					(System::Object^, System::EventArgs^);
		void	on_numeric2_value_changed					(System::Object^, System::EventArgs^);

	private:
		System::ComponentModel::Container^ components;
		viewer_editor^	m_editor;
		CheckBox^		checkBox1;
		CheckBox^		checkBox2;
		CheckBox^		checkBox3;
		Label^			label1;
		Label^			label2;
		NumericUpDown^	numericUpDown1;
		NumericUpDown^	numericUpDown2;
		Button^			button1;
		Button^			button2;
		bool			m_numeric_is_updating;
	}; // ref class animation_viewer_detailed_options_panel
} // namespace animation_editor
} // namespace xray
#endif // #ifndef XRAY_ANIMATION_EDITOR_VIEWER_EDITOR_H_INCLUDED