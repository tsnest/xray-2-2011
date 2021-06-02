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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4RTTIMCR_H
#define WM4RTTIMCR_H

//----------------------------------------------------------------------------
#define WM4_DECLARE_RTTI \
public: \
    static const Rtti TYPE; \
    virtual const Rtti& GetType () const { return TYPE; }
//----------------------------------------------------------------------------
#define WM4_DECLARE_TEMPLATE_RTTI \
public: \
    WM4_GRAPHICS_ITEM static const Rtti TYPE; \
    virtual const Rtti& GetType () const { return TYPE; }
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_RTTI(nsname,classname,baseclassname) \
    const Rtti classname::TYPE(#nsname"."#classname,&baseclassname::TYPE)
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_TEMPLATE_RTTI(nsname,classname,baseclassname) \
    template <> \
    const Rtti classname::TYPE(#nsname"."#classname,&baseclassname::TYPE)
//----------------------------------------------------------------------------

#endif
