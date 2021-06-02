////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_EDITOR_COLOR_HPP_INCLUDED
#define PROPERTY_EDITOR_COLOR_HPP_INCLUDED

public ref class property_editor_color : public System::Drawing::Design::UITypeEditor {
private:
	typedef System::Drawing::Design::UITypeEditor			inherited;
	typedef System::ComponentModel::ITypeDescriptorContext	ITypeDescriptorContext;
	typedef System::Drawing::Design::PaintValueEventArgs	PaintValueEventArgs;

public:
	virtual	bool	GetPaintValueSupported	(ITypeDescriptorContext^ context) override;
	virtual	void	PaintValue				(PaintValueEventArgs^ arguments) override;

public:
	typedef System::Drawing::Design::UITypeEditorEditStyle	UITypeEditorEditStyle;
	typedef System::IServiceProvider						IServiceProvider;
	typedef System::Object									Object;

	virtual UITypeEditorEditStyle	GetEditStyle(ITypeDescriptorContext^ context) override;
	virtual	Object^					EditValue	(
										ITypeDescriptorContext^ context,
										IServiceProvider^ provider,
										Object^ value
									) override;
}; // ref class property_editor_color

#endif // ifndef PROPERTY_EDITOR_COLOR_HPP_INCLUDED