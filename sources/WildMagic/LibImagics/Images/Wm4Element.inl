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
// Version: 4.0.1 (2006/09/21)

//----------------------------------------------------------------------------
inline unsigned short GetColor16 (unsigned char ucR, unsigned char ucG,
    unsigned char ucB)
{
    return (unsigned short)((unsigned(ucB) >> 3) 
        |  ((unsigned(ucG) >> 3) << 5 ) 
        |  ((unsigned(ucR) >> 3) << 10));
}
//----------------------------------------------------------------------------
inline unsigned char GetBlue16 (unsigned short usColor)
{ 
    return (unsigned char)(usColor & 0x001F); 
}
//----------------------------------------------------------------------------
inline unsigned char GetGreen16 (unsigned short usColor)
{ 
    return (unsigned char)((usColor & 0x03E0) >> 5); 
}
//----------------------------------------------------------------------------
inline unsigned char GetRed16 (unsigned short usColor)
{ 
    return (unsigned char)((usColor & 0x7C00) >> 10);
}
//----------------------------------------------------------------------------
inline unsigned int GetColor24 (unsigned char ucR, unsigned char ucG,
    unsigned char ucB)
{
    return (unsigned(ucB))
        |  ((unsigned(ucG)) << 8)
        |  ((unsigned(ucR)) << 16);
}
//----------------------------------------------------------------------------
inline unsigned char GetBlue24 (unsigned int uiColor)
{ 
    return (unsigned char)(uiColor & 0x000000FF); 
}
//----------------------------------------------------------------------------
inline unsigned char GetGreen24 (unsigned int uiColor)
{ 
    return (unsigned char)((uiColor & 0x0000FF00) >> 8); 
}
//----------------------------------------------------------------------------
inline unsigned char GetRed24 (unsigned int uiColor)
{ 
    return (unsigned char)((uiColor & 0x00FF0000) >> 16);
}
//----------------------------------------------------------------------------
