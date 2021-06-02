////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "attribute.h"
#include "tool_base.h"
#include "object_base.h"

namespace xray {
namespace editor {


void property_link::refresh()
{
	if(m_type==System::String::typeid)
	{
		string_getter_type^ getter	= safe_cast<string_getter_type^>(System::Delegate::CreateDelegate(string_getter_type::typeid, m_src_object, m_src_get_method));
		string_setter_type^ setter	= safe_cast<string_setter_type^>(System::Delegate::CreateDelegate(string_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	if(m_type==float3::typeid)
	{
		float3_getter_type^ getter	= safe_cast<float3_getter_type^>(System::Delegate::CreateDelegate(float3_getter_type::typeid, m_src_object, m_src_get_method));
		float3_setter_type^ setter	= safe_cast<float3_setter_type^>(System::Delegate::CreateDelegate(float3_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	if(m_type==color::typeid)
	{
		color_getter_type^ getter	= safe_cast<color_getter_type^>(System::Delegate::CreateDelegate(color_getter_type::typeid, m_src_object, m_src_get_method));
		color_setter_type^ setter	= safe_cast<color_setter_type^>(System::Delegate::CreateDelegate(color_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	if(m_type==int::typeid)
	{
		integer_getter_type^ getter	= safe_cast<integer_getter_type^>(System::Delegate::CreateDelegate(integer_getter_type::typeid, m_src_object, m_src_get_method));
		integer_setter_type^ setter	= safe_cast<integer_setter_type^>(System::Delegate::CreateDelegate(integer_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	if(m_type==float::typeid)
	{
		float_getter_type^ getter	= safe_cast<float_getter_type^>(System::Delegate::CreateDelegate(float_getter_type::typeid, m_src_object, m_src_get_method));
		float_setter_type^ setter	= safe_cast<float_setter_type^>(System::Delegate::CreateDelegate(float_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	if(m_type==bool::typeid)
	{
		boolean_getter_type^ getter	= safe_cast<boolean_getter_type^>(System::Delegate::CreateDelegate(boolean_getter_type::typeid, m_src_object, m_src_get_method));
		boolean_setter_type^ setter	= safe_cast<boolean_setter_type^>(System::Delegate::CreateDelegate(boolean_setter_type::typeid, m_dst_object, m_dst_set_method));
		setter						(getter());
	}else
	{
		R_ASSERT(0, "unsuppotrted type ", unmanaged_string(m_type->ToString()).c_str());
	}
}

link_storage::link_storage()
{
	m_storage		= gcnew System::Collections::ArrayList;
}

System::Collections::ArrayList^ link_storage::get_links(	object_base^ src_object, 
															System::String^ src_name, 
															object_base^ dst_object, 
															System::String^ dst_name)
{
	System::Collections::ArrayList^ result = gcnew System::Collections::ArrayList;
	
	for each (property_link^ lnk in m_storage)
	{
		if(	(!src_object	|| lnk->m_src_object==src_object)		&&
			(!src_name		|| lnk->m_src_property_name==src_name)	&&
			(!dst_object	|| lnk->m_dst_object==dst_object)		&&
			(!dst_name		|| lnk->m_dst_property_name==dst_name)	)

			result->Add(lnk);
	}	
	return result;
}

bool link_storage::has_links(	object_base^ src_object, 
								System::String^ src_name, 
								object_base^ dst_object, 
								System::String^ dst_name)
{
	for each (property_link^ lnk in m_storage)
	{
		if(	(!src_object	|| lnk->m_src_object==src_object)		&&
			(!src_name		|| lnk->m_src_property_name==src_name)	&&
			(!dst_object	|| lnk->m_dst_object==dst_object)		&&
			(!dst_name		|| lnk->m_dst_property_name==dst_name)	)

			return true;
	}	
	return false;
}

void link_storage::notify_on_changed(object_base^ src_object, System::String^ src_name)
{
	for each (property_link^ lnk in m_storage)
	{
		if(	(lnk->m_src_object==src_object)&&(lnk->m_src_property_name==src_name))
			lnk->refresh();		
	}	
}

System::Collections::ArrayList^	link_storage::get_links()
{
	return m_storage;
}

void link_storage::create_link(	object_base^ src_object, 
								System::String^ src_name, 
								object_base^ dst_object, 
								System::String^ dst_name)
{
	property_link^ link			= gcnew property_link;
	link->m_src_object			= src_object;
	link->m_dst_object			= dst_object;
	link->m_src_property_name 	= src_name;
	link->m_dst_property_name 	= dst_name;
	

	array<System::Reflection::PropertyInfo^>^	props;
	props							= src_object->GetType()->GetProperties();
	for(int p=0; p<props->Length; ++p)
	{
		System::Reflection::PropertyInfo^ pi	= props[p];

		array<System::Object^>^ attribs			= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		if(attribs->Length==0)					continue; // not a browsable attribute
		DisplayNameAttribute^ att_name			= safe_cast<DisplayNameAttribute^>(attribs[0]);
		if(att_name->DisplayName==src_name)
		{
			link->m_src_get_method				= pi->GetGetMethod();
			link->m_type						= pi->PropertyType;
			break;
		}
	}

	props							= dst_object->GetType()->GetProperties();
	for(int p=0; p<props->Length; ++p)
	{
		System::Reflection::PropertyInfo^ pi	= props[p];

		array<System::Object^>^ attribs			= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		if(attribs->Length==0)					continue; // not a browsable attribute
		DisplayNameAttribute^ att_name			= safe_cast<DisplayNameAttribute^>(attribs[0]);
		if(att_name->DisplayName==dst_name)
		{
			link->m_dst_set_method				= pi->GetSetMethod();
			ASSERT								(link->m_type == pi->PropertyType);
			break;
		}
	}
	ASSERT						(link->m_src_get_method!=nullptr);
	ASSERT						(link->m_dst_set_method!=nullptr);
	m_storage->Add				(link);
	link->refresh				();
}

void link_storage::remove_link(	object_base^ src_object, 
								System::String^ src_name, 
								object_base^ dst_object, 
								System::String^ dst_name)
{

	System::Collections::ArrayList^ curr = get_links(src_object, src_name, dst_object, dst_name);
	for each (property_link^ lnk in curr)
	{
		m_storage->Remove(lnk);
	}
}

void link_storage::clear_all(	)
{
	m_storage->Clear();
}

void library_name_ui_type_editor::run_editor( xray::editor::wpf_controls::property_editors::property^ prop )
{
	wpf_controls::property_container^ container = safe_cast<wpf_controls::property_container^>(prop->property_owners[0]);
	object_base^ o = safe_cast<object_base^>(container->owner);
	
	System::String^ selected	= nullptr;
	if( editor_base::resource_chooser::choose( o->owner_tool()->name(), 
														o->get_library_name(), 
														nullptr, 
														selected,
														true)
		)
	{
		for(int i=0; i<prop->property_owners->Count; ++i)
		{
			wpf_controls::property_container^ container = safe_cast<wpf_controls::property_container^>(prop->property_owners[i]);
			object_base^ o = safe_cast<object_base^>(container->owner);
			o->set_library_name						( selected );
		}

	}
}

} // namespace editor
} // namespace xray
