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
// Version: 4.0.1 (2006/08/07)

#ifndef WM4COLLAPSERECORDARRAY_H
#define WM4COLLAPSERECORDARRAY_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"
#include "Wm4CollapseRecord.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM CollapseRecordArray : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    CollapseRecordArray (int iQuantity = 0, CollapseRecord* akArray = 0);
    CollapseRecordArray (const CollapseRecordArray& rkShared);
    virtual ~CollapseRecordArray ();

    CollapseRecordArray& operator= (const CollapseRecordArray& rkShared);

    int GetQuantity () const;
    CollapseRecord* GetData () const;
    operator const CollapseRecord* () const;
    operator CollapseRecord* ();
    const CollapseRecord& operator[] (int i) const;
    CollapseRecord& operator[] (int i);

protected:
    int m_iQuantity;
    CollapseRecord* m_akArray;
};

WM4_REGISTER_STREAM(CollapseRecordArray);
typedef Pointer<CollapseRecordArray> CollapseRecordArrayPtr;
#include "Wm4CollapseRecordArray.inl"

}

#endif
