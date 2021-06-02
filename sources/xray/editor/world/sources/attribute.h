////////////////////////////////////////////////////////////////////////////
//	Created		: 11.06.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ATTRIBUTE_H_INCLUDED
#define ATTRIBUTE_H_INCLUDED

#include "project_defines.h"
using System::AttributeTargets;

using System::ComponentModel::DisplayNameAttribute;
using System::ComponentModel::DescriptionAttribute;
using System::ComponentModel::CategoryAttribute;
using System::ComponentModel::DefaultValueAttribute;
using System::ComponentModel::ReadOnlyAttribute;
using System::ComponentModel::MergablePropertyAttribute;

typedef System::Action<xray::editor::wpf_controls::property_grid::property^>	button_delegate;

public enum class enum_connection : int {in=0, out=1, inout=2 };
[System::AttributeUsage(AttributeTargets::Property, AllowMultiple=false)]
public ref class ConnectionAttribute : public System::Attribute 
{
public:
	ConnectionAttribute	(enum_connection t):m_type(t){}
	enum_connection		m_type;
};//ConnectionAttribute

namespace xray {
namespace editor {

public ref struct property_link
{
	System::String^			m_src_property_name;
	System::String^			m_dst_property_name;
	object_base^			m_src_object;
	object_base^			m_dst_object;
	System::Reflection::MethodInfo^				m_src_get_method;
	System::Reflection::MethodInfo^				m_dst_set_method;
	System::Type^			m_type;

	void					refresh();
}; //property_link

public ref class link_storage
{
public:
										link_storage();
	System::Collections::ArrayList^		get_links(	object_base^ src_object, 
													System::String^ src_name, 
													object_base^ dst_object, 
													System::String^ dst_name);

	bool								has_links(	object_base^ src_object, 
													System::String^ src_name, 
													object_base^ dst_object, 
													System::String^ dst_name);

	System::Collections::ArrayList^		get_links(	);
	void								clear_all(	);

	void								create_link(object_base^ src_object, 
													System::String^ src_name, 
													object_base^ dst_object, 
													System::String^ dst_name);
	void								remove_link(object_base^ src_object, 
													System::String^ src_name, 
													object_base^ dst_object, 
													System::String^ dst_name);
	void								notify_on_changed(	object_base^ src_object, 
															System::String^ src_name);
private:
	System::Collections::ArrayList^		m_storage;
}; //link_storaga


public ref class library_name_ui_type_editor : public xray::editor::wpf_controls::property_editors::i_external_property_editor
{
public:
	virtual	void			run_editor	( xray::editor::wpf_controls::property_editors::property^ prop );
}; // ref class library_name_ui_type_editor

} // namespace editor
} // namespace xray

#endif // #ifndef ATTRIBUTE_H_INCLUDED
