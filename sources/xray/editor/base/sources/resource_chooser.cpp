////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_chooser.h"

namespace xray{
namespace editor_base{

void resource_chooser::register_chooser( chooser^ c )
{
	R_ASSERT(!m_choosers.ContainsKey(c->id()));
	m_choosers[c->id()] = c;
}

void resource_chooser::unregister_chooser( chooser^ c )
{
	R_ASSERT(m_choosers.ContainsKey(c->id()));
	m_choosers.Remove(c->id());
}

bool resource_chooser::choose( System::String^ id, 
								System::String^ current, 
								System::String^ filter, 
								System::String^% result,
								bool read_only )
{
	R_ASSERT(m_choosers.ContainsKey(id));
	chooser^ c = m_choosers[id];
	return c->choose	( current, filter, result, read_only );
}

void resource_chooser::set_shared_context( System::String^ key, 
											System::Object^ value )
{
	if(m_shared_context.ContainsKey(key))
		m_shared_context[key] = value;
	else
		m_shared_context.Add(key, value);
}

System::Object^ resource_chooser::get_shared_context( System::String^ key )
{
	if(m_shared_context.ContainsKey(key))
		return m_shared_context[key];
	else
		return nullptr;
}

} //namespace xray
} //namespace editor_base
