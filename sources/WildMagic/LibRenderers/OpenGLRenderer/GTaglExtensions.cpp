// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.3.0 (2007/03/17)

#include <mach-o/dyld.h>
#include <cstdlib>
#include <cstring>

extern "C"
{

//----------------------------------------------------------------------------
void* GTGetFunctionPointer (const char* acFunction)
{
    NSSymbol kSymbol;
    char* acSymbolName;

    // Prepend a '_' for the Unix C symbol mangling convention.
    acSymbolName = (char*)malloc(strlen((const char*)acFunction) + 2);
    strcpy(acSymbolName+1,(const char*)acFunction);
    acSymbolName[0] = '_';

    kSymbol = 0;
    if (NSIsSymbolNameDefined(acSymbolName))
	{
        kSymbol = NSLookupAndBindSymbol(acSymbolName);
	}

    free(acSymbolName);
    return kSymbol ? NSAddressOfSymbol(kSymbol) : 0;
}
//----------------------------------------------------------------------------

} // extern "C"
