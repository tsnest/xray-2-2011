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
// Version: 4.7.0 (2008/09/15)

#ifndef WM4UNORDEREDSET_H
#define WM4UNORDEREDSET_H

// An unordered set of objects stored in contiguous memory.  The type T must
// have the following member functions:
//   T::T();
//   T::~T();
//   T& operator= (const T&);
//   bool operator== (const T&) const;

#include "Wm4System.h"

namespace Wm4
{

template <class T>
class UnorderedSet
{
public:
    UnorderedSet (int iMaxQuantity = 0, int iGrow = 0);
    UnorderedSet (const UnorderedSet& rkSet);
    ~UnorderedSet ();

    void Reset (int iMaxQuantity = 0, int iGrow = 0);
    void Clear ();
    UnorderedSet& operator= (const UnorderedSet& rkSet);

    int GetMaxQuantity () const;
    int GetGrow () const;

    int GetQuantity () const;
    const T& Get (int i) const;
    T& operator[] (int i);

    bool Exists (const T& rtElement) const;
    bool Insert (const T& rtElement);
    int Append (const T& rtElement);
    bool Remove (const T& rtElement, int* piOld = 0, int* piNew = 0);
    bool RemoveAt (int i, int* piOld = 0, int* piNew = 0);
    void Compactify ();

    enum { DEFAULT_GROW = 8 };

protected:
    int m_iQuantity, m_iMaxQuantity, m_iGrow;
    T* m_atElement;
};

#include "Wm4UnorderedSet.inl"

}

#endif
