////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "path_parts.h"

using xray::logging::path_parts;

path_parts::path_parts	(pcstr initiator) : m_index(0), m_current_element(0) 
{
	ASSERT									( initiator );
	if ( * initiator != initiator_separator )
		add_part							( initiator );
	add_part								(0);
}

void path_parts::add_part		( pcstr const part )
{
	ASSERT					( !part || *part );
	if ( m_parts.empty() )
		m_current_element	= part;

	m_parts.push_back		( part );
}

void path_parts::to_next_element()
{
	m_current_element		= strchr( m_current_element, initiator_separator );
	if ( m_current_element && m_current_element[1] ) {
		++m_current_element; // next element in this part
		return;
	}

	m_current_element		= m_parts[++m_index]; // next part of path
}

void path_parts::concat2buffer(xray::buffer_string& buffer)
{
	int cur_index			= 0;

	while ( m_parts[cur_index] ) {
		buffer				+= m_parts[cur_index];
		++cur_index;
	}

	if ( buffer.length() && *buffer.rbegin() == initiator_separator )
		buffer.set_length	( buffer.length() - 1 );
}