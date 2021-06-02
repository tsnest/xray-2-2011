////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/strings_functions.h>

namespace xray {
namespace strings {

bool   convert_string_to_number (pcstr const string, float * const out_result)
{
	CURE_ASSERT					(string, return false, "1st argument is null pointer");
	CURE_ASSERT					(out_result, return false, "2nd argument is null pointer");

	float const number_value	= (float)atof( (pstr)string );
	* out_result				= number_value;
	if ( number_value != 0.f )
		return					true;

	return
		strings::equal(string, "0")	|| 
		strings::equal(string, "0.") || 
		strings::equal(string, "0.0");
}

bool   compare_with_wildcards (pcstr wild, pcstr string) 
{
    pcstr	cp = 0, mp = 0;

    while ( (*string) && (*wild != '*') ) 
	{
		if ( (*wild != *string) && (*wild != '?') ) 
            return		0;

        ++wild;
        ++string;
    }

	while ( *string )
	{
        if ( *wild == '*' ) 
		{
            if ( !*++wild ) 
				return 1;

            mp		=	wild;
            cp		=	string + 1;
        }
		else if ( (*wild == *string) || (*wild == '?') ) 
		{
            ++wild;
            ++string;
        } 
		else 
		{
            wild	=	mp;
            string	=	cp++;
        }
    }

	while ( *wild == '*' )
		++wild;

	return				!*wild;
}

} // namespace xray
} // namespace strings