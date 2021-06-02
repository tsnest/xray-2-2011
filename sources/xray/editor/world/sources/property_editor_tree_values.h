////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_EDITOR_TREE_VALUES_HPP_INCLUDED
#define PROPERTY_EDITOR_TREE_VALUES_HPP_INCLUDED

#include "property_holder_include.h"

namespace xray {
namespace editor {
	ref class window_tree_values;
} // namespace editor
} // namespace xray

public ref class property_editor_tree_values : public System::Drawing::Design::UITypeEditor {
private:
	typedef System::Drawing::Design::UITypeEditor			inherited;
	typedef System::Drawing::Design::UITypeEditorEditStyle	UITypeEditorEditStyle;
	typedef System::Windows::Forms::OpenFileDialog			OpenFileDialog;
	typedef Flobbster::Windows::Forms::PropertyBag			PropertyBag;
	typedef PropertyBag::PropertySpecDescriptor				PropertySpecDescriptor;
	typedef System::ComponentModel::ITypeDescriptorContext	ITypeDescriptorContext;
	typedef System::IServiceProvider						IServiceProvider;
	typedef System::Object									Object;

public:
									property_editor_tree_values	();
	virtual UITypeEditorEditStyle	GetEditStyle				(ITypeDescriptorContext^ context) override;
	virtual	System::Object^			EditValue					(
										ITypeDescriptorContext^ context,
										IServiceProvider^ provider,
										Object ^value
									) override;

private:
	xray::editor::window_tree_values^	m_dialog;
}; // ref class property_editor_tree_values

#endif // ifndef PROPERTY_EDITOR_TREE_VALUES_HPP_INCLUDED