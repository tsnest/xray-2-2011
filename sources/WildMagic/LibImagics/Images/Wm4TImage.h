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

#ifndef WM4TIMAGE_H
#define WM4TIMAGE_H

#include "Wm4ImagicsLIB.h"
#include "Wm4ImageConvert.h"
#include "Wm4Lattice.h"

namespace Wm4
{

// The class T is intended to be a wrapper of native data (int, float, char,
// etc.).  The code uses memcpy, memcmp, and memset on the array of T values.
// Class T must have the following member functions:
//   T::T ()
//   T& T::operator= (T)
//   static const char* GetRTTI ()
// The static member function returns a string that is used for streaming.


template <class T>
class TImage : public Lattice
{
public:
    // Construction and destruction.  TImage accepts responsibility for
    // deleting the input arrays.
    TImage (int iDimensions, int* aiBound, T* atData = 0);
    TImage (const TImage& rkImage);
    TImage (const char* acFilename);
    virtual ~TImage ();

    // data access
    T* GetData () const;
    T& operator[] (int i) const;

    // assignment
    TImage& operator= (const TImage& rkImage);
    TImage& operator= (T tValue);

    // comparison
    bool operator== (const TImage& rkImage) const;
    bool operator!= (const TImage& rkImage) const;

    // streaming
    bool Load (const char* acFilename);
    bool Save (const char* acFilename) const;

protected:
    // for deferred creation of bounds
    TImage (int iDimensions);
    void SetData (T* atData);

    T* m_atData;
};

#include "Wm4TImage.inl"

}

#endif
