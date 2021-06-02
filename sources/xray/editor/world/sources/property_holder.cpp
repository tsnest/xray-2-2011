////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_holder.h"
#include "property_container.h"

using Flobbster::Windows::Forms::PropertySpec;
using System::String;
using xray::editor::property_holder_collection;
using xray::editor::engine;
using xray::editor::property_holder_holder;
using System::Collections::IDictionary;
using System::Collections::Generic::Dictionary;
using System::Collections::Generic::KeyValuePair;
using xray::editor::controls::property_value;

typedef property_holder::collection_type		collection_type;

property_holder::property_holder				(
//		xray::editor::engine* engine,
		LPCSTR display_name,
		property_holder_collection* collection,
		xray::editor::property_holder_holder* holder
	) :
//	m_engine					(engine),
	m_display_name				( gcnew System::String( display_name ) ),
	m_collection				(collection),
	m_holder					(holder),
	m_disposing					(false)
{
	m_container					= gcnew property_container(this, nullptr);
}

property_holder::~property_holder				()
{
	if (m_disposing)
		return;

	m_disposing					= true;
	delete						(m_container);
}

void property_holder::on_dispose				()
{
	if (m_disposing)
		return;

	if ( !m_collection )
		return;

	ASSERT						(m_collection);
	int							index = m_collection->index(this);
	if (index < 0) {
		m_collection->destroy	(this);
		return;
	}

	ASSERT						((u32)index < m_collection->size());
	m_collection->erase			(index);
}

property_container ^property_holder::container	()
{
	return						(m_container);
}

//engine& property_holder::engine					()
//{
//	ASSERT						(m_engine);
//	return						(*m_engine);
//}

property_holder_holder*	property_holder::holder	()
{
	return						(m_holder);
}

String^	property_holder::display_name			()
{
	return						(m_display_name);
}

collection_type* property_holder::collection	()
{
	return						(m_collection);
}

void property_holder::clear						()
{
	m_container->clear			();
}

int property_holder::get_values_count (System::String^ property_id){
	int count = 0;

	Dictionary<PropertySpec^, property_value^>^ properties		= m_container->properties( );
	for each (KeyValuePair<PropertySpec^, property_value^>^ i in properties) {
		property_container::PropertySpec^ property_spec	= i->Key;
		if ( property_spec->Name != property_id )
			continue;
		count++;
	}

	return						count;
}

System::Object^ property_holder::get_value		( System::String^ property_id )
{
	Dictionary<PropertySpec^, property_value^>^ properties		= m_container->properties( );
	for each (KeyValuePair<PropertySpec^, property_value^>^ i in properties) {
		property_container::PropertySpec^ property_spec	= i->Key;
		if ( property_spec->Name != property_id )
			continue;

		xray::editor::controls::property_value^ value	= i->Value;
		return					value->get_value( );
	}

	return						nullptr;
}

 void property_holder::remove_property	( System::String^ property_id )
 {
 
 	Dictionary<PropertySpec^, property_value^>^ properties		= m_container->properties( );
 	for each (KeyValuePair<PropertySpec^, property_value^>^ i in properties) {
 		property_container::PropertySpec^ property_spec	= i->Key;
 		if ( property_spec->Name != property_id )
 			continue;
 		container()->remove_property(property_spec);	
 		return;
 	}
 
 	
 }