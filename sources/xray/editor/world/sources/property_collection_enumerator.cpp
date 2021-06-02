////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_collection_enumerator.h"
#include "property_holder.h"

using System::Object;
using System::InvalidOperationException;

property_collection_enumerator::property_collection_enumerator	(collection_type* collection) :
	m_collection				(collection),
	m_cursor					(-1)
{
}

void property_collection_enumerator::Reset						()
{
	m_cursor					= -1;
}

bool property_collection_enumerator::MoveNext					()
{
	if (m_cursor < (int)m_collection->size())
		++m_cursor;

	return						(m_cursor != (int)m_collection->size());
}

Object^ property_collection_enumerator::Current::get			()
{
	if (m_cursor < 0)
		throw					(gcnew InvalidOperationException());

	if (m_cursor >= (int)m_collection->size())
		throw					(gcnew InvalidOperationException());

	xray::editor::property_holder*	holder_raw = m_collection->item((u32)m_cursor);
	property_holder*			holder = dynamic_cast<property_holder*>(holder_raw);
	ASSERT						(holder);
	return						(holder->container());
}