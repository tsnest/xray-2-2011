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

#ifndef WM4RTTI_H
#define WM4RTTI_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"
#include "Wm4RttiMCR.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Rtti
{
public:
    // The name must be unique among all objects in the system.  In the Wm4
    // namespace, a class Foo should use "Wm4.Foo".  If an application has
    // another namespace, SomeName, then the name should be "SomeName.Foo".
    Rtti (const char* acName, const Rtti* pkBaseType);
    ~Rtti ();

    const char* GetName () const;
    int GetDiskUsed () const;

    bool IsExactly (const Rtti& rkType) const;
    bool IsDerived (const Rtti& rkType) const;

private:
    const char* m_acName;
    const Rtti* m_pkBaseType;
};

#include "Wm4Rtti.inl"

}

#endif
