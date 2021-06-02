////////////////////////////////////////////////////////////////////////////
//	Module 		: string_functions.cpp
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : string functions
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

inline bool white_character			(char const& character)
{
	return		character <= ' ';
}

pstr sz_trim_left					(pstr string)
{
	ASSERT		(string, "null string passed");

	pstr		I = string;
	for ( ; *I && white_character(*I); ++I);

	if (!*I) {
		*string	= 0;
		return	string;
	}

	if (I == string)
		return	string;

	pstr		J = string;
	for ( ; *I; ++I, ++J)
		*J		= *I;

	*J			= 0;
	return		string;
}

pstr sz_trim_right					(pstr string)
{
	ASSERT		(string, "null string passed");

	pstr		B = string;
	pstr		E = string + sz_len(string);
	pstr		I = E - 1;
	for ( ; (I >= B) && white_character(*I); --I);

	if (I < B) {
		*string	= 0;
		return	string;
	}

	*(I + 1)	= 0;
	return		string;
}

void string::initialize				()
{
	ASSERT					(!g_shared_string_manager, "shared string manager has been already initialized");
	g_shared_string_manager	= cs_new<shared_string_manager>();
}

void string::uninitialize			()
{
	cs_delete				(g_shared_string_manager);
}
