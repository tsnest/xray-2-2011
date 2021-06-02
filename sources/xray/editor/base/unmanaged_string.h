////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef UNMANAGED_STRING_H_INCLUDED
#define UNMANAGED_STRING_H_INCLUDED

class unmanaged_string : private boost::noncopyable {
public:
	inline			unmanaged_string	( System::String^ string );
	inline			~unmanaged_string	( );
	inline	pcstr	c_str				( ) const;

private:
	pstr	m_string;
}; // class unmanaged_string

#include <xray/editor/base/unmanaged_string_inline.h>

#endif // #ifndef UNMANAGED_STRING_H_INCLUDED