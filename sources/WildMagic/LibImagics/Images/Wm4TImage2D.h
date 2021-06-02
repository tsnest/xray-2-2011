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

#ifndef WM4TIMAGE2D_H
#define WM4TIMAGE2D_H

#include "Wm4ImagicsLIB.h"
#include "Wm4TImage.h"

namespace Wm4
{

template <class T>
class TImage2D : public TImage<T>
{
public:
    // Construction and destruction.  TImage2D accepts responsibility for
    // deleting the input data array.
    TImage2D (int iXBound, int iYBound, T* atData = 0);
    TImage2D (const TImage2D& rkImage);
    TImage2D (const char* acFilename);

    // data access
    T& operator() (int iX, int iY) const;

    // conversion between 2D coordinates and 1D indexing
    int GetIndex (int iX, int iY) const;
    void GetCoordinates (int iIndex, int& riX, int& riY) const;

    // assignment
    TImage2D& operator= (const TImage2D& rkImage);
    TImage2D& operator= (T tValue);

protected:
    using TImage<T>::SetData;
    using TImage<T>::m_atData;
};

#include "Wm4TImage2D.inl"

}

#endif
