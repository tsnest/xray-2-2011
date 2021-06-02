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

#ifndef WM4SOFTARRAY_H
#define WM4SOFTARRAY_H

#include "Wm4SoftRendererLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <typename Type>
class SoftArray
{
public:
    SoftArray (int iMaxQuantity);
    SoftArray (const SoftArray& rkObject);
    ~SoftArray ();

    SoftArray& operator= (const SoftArray& rkObject);

    int GetQuantity () const;
    int GetMaxQuantity () const;
    Type* GetData ();
    const Type* GetData () const;

    void EnsureCapacity (int iRequestedMaxQuantity);
    void Clear ();
    void Append (Type tValue);
    Type Get (int i) const;

private:
    int m_iQuantity, m_iMaxQuantity;
    Type* m_atData;
};

#include "Wm4SoftArray.inl"

}

#endif
