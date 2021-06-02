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

#ifndef WM4TSHAREDARRAY_H
#define WM4TSHAREDARRAY_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"

namespace Wm4
{

template <class T>
class TSharedArray : public Object
{
    WM4_DECLARE_TEMPLATE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_TEMPLATE_STREAM;

public:
    TSharedArray (int iQuantity = 0, T* atArray = 0);
    TSharedArray (const TSharedArray& rkShared);
    virtual ~TSharedArray ();

    TSharedArray& operator= (const TSharedArray& rkShared);

    int GetQuantity () const;
    T* GetData () const;
    operator const T* () const;
    operator T* ();
    const T& operator[] (int i) const;
    T& operator[] (int i);

    // This will not cause a reallocation of the array.  The idea is to
    // allow only some of the data to be considered valid.  The caller is
    // responsible for remembering the total quantity that was passed to the
    // constructor.
    void SetActiveQuantity (int iActiveQuantity);

protected:
    int m_iQuantity;
    T* m_atArray;
};

#include "Wm4TSharedArray.inl"

}

#endif
