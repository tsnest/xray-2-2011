////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "editor_world.h"

using Flobbster::Windows::Forms::PropertySpecEventHandler;
using System::Collections::Hashtable;
using System::Collections::IDictionary;
using System::String;
using System::Collections::ArrayList;
using xray::editor::controls::property_value;

property_container::property_container			(
		property_holder* holder,
		property_container_holder^ container_holder
	) :
	m_holder					(holder),
	m_container_holder			(container_holder),
	m_properties				(gcnew Hashtable()),
	m_ordered_properties		(gcnew ArrayList())
{
	GetValue					+= gcnew PropertySpecEventHandler(this, &property_container::get_value_handler);
	SetValue					+= gcnew PropertySpecEventHandler(this, &property_container::set_value_handler);
}

property_container::~property_container			()
{
	this->!property_container	();
}

property_container::!property_container			()
{
	clear						();

	if (!m_holder)
		return;

	property_holder*			holder = dynamic_cast<property_holder*>(m_holder);
	ASSERT						(holder);

	holder->on_dispose			();
}

property_holder &property_container::holder		()
{
	ASSERT						(m_holder);
	return						(*m_holder);
}

property_container_holder% property_container::container_holder	()
{
	ASSERT						(m_container_holder);
	return						(*m_container_holder);
}

bool property_container::equal_category			(String^ new_category, String^ old_category)
{
	ASSERT						(!new_category->Length || (new_category[0] != '\t'));
	if (!old_category->Length || (old_category[0] != '\t'))
		return					(new_category == old_category);

	for (u32 i=0, n=old_category->Length; i<n; ++i) {
		if (old_category[i] == '\t')
			continue;

		return					(new_category == old_category->Substring(i, n - i));
	}

	UNREACHABLE_CODE(return false);
}

String^ property_container::update_categories	(String^ new_category)
{
	for each (PropertySpec^ i in m_ordered_properties) {
		String^					category = i->Category;
		if (!equal_category(new_category, category))
			continue;

		return					(category);
	}

	for each (PropertySpec^ i in m_ordered_properties)
		i->Category				= "\t" + i->Category;

	return						(new_category);
}

void property_container::try_update_name		(PropertySpec^ description, String^ name)
{
	ASSERT						(!!name->Length);
	ASSERT						((name[0] != '\t'));
	
	String^						description_name = description->Name;
	ASSERT						(!!description_name->Length);
	if (description_name[0] != '\t') {
		if (name != description_name)
			return;
		
		description->Name		= "\t" + description_name;
		return;
	}

	for (u32 i=0, n=description_name->Length; i<n; ++i) {
		if (description_name[i] == '\t')
			continue;

		if (name != description_name->Substring(i, n - i))
			return;
		
		description->Name		= "\t" + description->Name;
		return;
	}

	UNREACHABLE_CODE();
}

void property_container::update_names			(String^ name)
{
	bool						found = false;
	for each (PropertySpec^ i in m_ordered_properties) {
		if (i->Name != name)
			continue;

		found					= true;
		break;
	}

	if (!found)
		return;

	for each (PropertySpec^ i in m_ordered_properties)
		try_update_name			(i, name);
}

void property_container::add_property			(PropertySpec^ description, property_value^ value)
{
	ASSERT						(!m_properties[description]);

	description->Category		= update_categories(description->Category);
	update_names				(description->Name);
	m_properties[description]	= value;
	Properties->Add				(description);
	m_ordered_properties->Add	(description);
}

void property_container::remove_property(PropertySpec^ description){
	m_properties->Remove(description);
	Properties->Remove(description);
	m_ordered_properties->Remove(description);	
}

void property_container::add_property	(
		PropertySpec^ description,
		property_value^ value,
		readonly_enum read_only,
		notify_parent_on_change_enum notify_parent,
		password_char_enum password,
		refresh_grid_on_change_enum refresh_grid
	)
{
	u32	count					= 4;
	u32 const current_length	= description->Attributes ? description->Attributes->Length : 0;
	u32 const new_length		= count + current_length;

	array< System::Attribute^ >^attributes = gcnew array< System::Attribute^ >( new_length );

	for ( u32 i = 0; i < current_length; ++i )
		attributes[i]			= description->Attributes[i];

	attributes[current_length+0]	= gcnew System::ComponentModel::ReadOnlyAttribute( 
											(read_only == property_holder::property_read_only) 
											);
	attributes[current_length+1]	= gcnew System::ComponentModel::NotifyParentPropertyAttribute( 
											(notify_parent == property_holder::notify_parent_on_change) 
											);
	attributes[current_length+2]	= gcnew System::ComponentModel::PasswordPropertyTextAttribute( 
											(password == property_holder::password_char) 
											);
	attributes[current_length+3]	= gcnew System::ComponentModel::RefreshPropertiesAttribute( 
											(refresh_grid==property_holder::refresh_grid_on_change) ? 
												System::ComponentModel::RefreshProperties::All : 
												System::ComponentModel::RefreshProperties::None
											);

	description->Attributes		= attributes;

	add_property				( description, value );
}

property_value^ property_container::value		(PropertySpec^ description)
{
	ASSERT						(m_properties[description]);
	return						(safe_cast<property_value^>(m_properties[description]));
}

IDictionary% property_container::properties		()
{
	ASSERT						(m_properties);
	return						(*m_properties);
}

ArrayList% property_container::ordered_properties	()
{
	ASSERT						(m_ordered_properties);
	return						(*m_ordered_properties);
}

void property_container::get_value_handler		(Object^ sender, PropertySpecEventArgs^ e)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );

	property_value				^value = safe_cast<property_value^>(m_properties[e->Property]);
	ASSERT						(value);
	e->Value					= value->get_value();
}

void property_container::set_value_handler		(Object^ sender, PropertySpecEventArgs^ e)
{
	XRAY_UNREFERENCED_PARAMETER	( sender );

	property_value				^value = safe_cast<property_value^>(m_properties[e->Property]);
	ASSERT						(value);
	value->set_value			(e->Value);
}

void property_container::clear					()
{
	m_ordered_properties->Clear	();
	Properties->Clear			();

	while ( m_properties->Count ) {
		for each (System::Collections::DictionaryEntry^ i in m_properties) {
			m_properties->Remove	( i->Key );
			property_value^	value	= safe_cast<property_value^>(i->Value);
			delete					value;
			break;
		}
	}
	m_properties->Clear			();
}

property_restrictor::property_restrictor()
:m_property_holder(NULL)
{
	m_restricted_items = gcnew System::Collections::ArrayList;
}

void property_restrictor::add_property(System::String^ property_name, bool b_readonly)
{
	ro_property_wrapper^ pw			= gcnew ro_property_wrapper(property_name, b_readonly);
	m_restricted_items->Add			(pw);
	m_property_holder->add_bool		(property_name, 
									"Allow change", 
									"property name", 
									false, 
									gcnew boolean_getter_type(pw, &ro_property_wrapper::get_readwrite),
									gcnew boolean_setter_type(pw, &ro_property_wrapper::set_readwrite),
									property_holder::property_read_write,
									property_holder::do_not_notify_parent_on_change,
									property_holder::no_password_char,
									property_holder::do_not_refresh_grid_on_change);
}

bool property_restrictor::is_readonly(System::String^ property_name)
{
	for each (ro_property_wrapper^ w in m_restricted_items)
	{
		if(w->m_property_name==property_name)
		{
			return !w->get_readwrite();
			break;
		}
	}
	return false;
}

void property_restrictor::reset()
{
	m_restricted_items->Clear	();
	m_property_holder->clear	();
}

void property_restrictor::apply_to(property_holder* holder)
{
	property_container^ container		= holder->container();
	property_container::IDictionary% properties = container->properties();

	for each (System::Collections::DictionaryEntry^ i in properties)
	{
		property_container::PropertySpec^ spec	= safe_cast<property_container::PropertySpec^>(i->Key);
		System::String^ name					= spec->Name;
		if(is_readonly(name))
		{
			ReadOnlyAttribute^ ro_attribute		= gcnew ReadOnlyAttribute(true);

			u32 att_count						= spec->Attributes->Length;
			for(u32 i=0; i<att_count; ++i)
			{
				if(spec->Attributes[i]->TypeId==ro_attribute->TypeId)
				{
					spec->Attributes[i] = ro_attribute;
					break;
				}
			}
		}
	}
}

void property_restrictor::create_from(property_holder* holder)
{
	property_container^ container				= holder->container();
	property_container::IDictionary% properties = container->properties();

	ReadOnlyAttribute^ ro_attribute				= gcnew ReadOnlyAttribute(false);
	
	for each (System::Collections::DictionaryEntry^ i in properties)
	{
		property_container::PropertySpec^ spec	= safe_cast<property_container::PropertySpec^>(i->Key);
		System::String^ property_name			= spec->Name;
		u32 att_count							= spec->Attributes->Length;
		for(u32 i=0; i<att_count; ++i)
		{
			if(spec->Attributes[i]->TypeId==ro_attribute->TypeId)
			{
				ReadOnlyAttribute^ att	= dynamic_cast<ReadOnlyAttribute^>(spec->Attributes[i]);
				add_property			(property_name, !att->IsReadOnly);
				break;
			}
		}
	}
}

void property_restrictor::load(xray::configs::lua_config_value const& lv)
{
	reset();
	if(!lv.value_exists("restrictions"))
		return;

	xray::configs::lua_config_value t = lv["restrictions"];


	xray::configs::lua_config::const_iterator	it		= t.begin();
	xray::configs::lua_config::const_iterator	it_e	= t.end();
	for ( ;it!=it_e; ++it) 
	{
		pcstr prop_name		= *it;
		add_property		(gcnew System::String(prop_name), false);
	}
}

void property_restrictor::save(xray::configs::lua_config_value lv)
{
	xray::configs::lua_config_value t = lv["restrictions"];
	for each (ro_property_wrapper^ w in m_restricted_items)
	{
		if(w->get_readwrite())
			continue;

		xray::editor::unmanaged_string us		(w->m_property_name);
		t[(pcstr)(us.c_str())] = us.c_str();
	}
}

void property_restrictor::in_create()
{
	m_property_holder = NEW(property_holder)("restricted props", NULL, NULL);
}

void property_restrictor::in_destroy()
{
	xray::editor::property_holder* ph	= m_property_holder;
	DELETE								(ph);
	m_property_holder					= NULL;
}
