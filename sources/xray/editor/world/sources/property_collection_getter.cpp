////////////////////////////////////////////////////////////////////////////
//	Created 	: 08.01.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_collection_getter.h"

property_collection_getter::property_collection_getter		(collection_getter_type const& getter) :
	m_getter	(NEW(collection_getter_type)(getter))
{
}

property_collection_getter::~property_collection_getter		()
{
	this->!property_collection_getter	();
}

property_collection_getter::!property_collection_getter		()
{
	DELETE		(m_getter);
}

property_collection_getter::collection_type* property_collection_getter::collection	()
{
	return		((*m_getter)());
}