////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_collection.h"
#include "property_collection_enumerator.h"
#include "property_container.h"

using System::Object;
using System::Collections::IEnumerator;

property_collection::property_collection		(collection_type* collection) :
	m_collection				(collection)
{
}

property_collection::~property_collection		()
{
	this->!property_collection	();
}

property_collection::!property_collection		()
{
}

property_collection::collection_type* property_collection::collection	()
{
	return						(m_collection);
}