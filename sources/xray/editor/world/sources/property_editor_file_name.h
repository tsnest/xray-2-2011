////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_EDITOR_FILE_NAME_HPP_INCLUDED
#define PROPERTY_EDITOR_FILE_NAME_HPP_INCLUDED

#include "property_holder_include.h"

namespace CustomControls {
namespace Controls {
	ref class OpenFileDialogEx;
} // namespace Controls
} // namespace CustomControls

public ref class property_editor_file_name : public System::Drawing::Design::UITypeEditor {

private:
	typedef System::Drawing::Design::UITypeEditor			inherited;
	typedef System::Drawing::Design::UITypeEditorEditStyle	UITypeEditorEditStyle;
	typedef System::Windows::Forms::OpenFileDialog			OpenFileDialog;
	typedef Flobbster::Windows::Forms::PropertyBag			PropertyBag;
	typedef PropertyBag::PropertySpecDescriptor				PropertySpecDescriptor;
	typedef System::ComponentModel::ITypeDescriptorContext	ITypeDescriptorContext;
	typedef System::IServiceProvider						IServiceProvider;
	typedef System::Object									Object;
	typedef CustomControls::Controls::OpenFileDialogEx		OpenFileDialogEx;

public:
									property_editor_file_name	();
	virtual UITypeEditorEditStyle	GetEditStyle				(ITypeDescriptorContext^ context) override;
	virtual	Object^					EditValue					(
										ITypeDescriptorContext^ context,
										IServiceProvider^ provider,
										Object ^value
									) override;

private:
	OpenFileDialog^					m_dialog;
}; // ref class property_editor_file_name

#endif // ifndef PROPERTY_EDITOR_FILE_NAME_HPP_INCLUDED