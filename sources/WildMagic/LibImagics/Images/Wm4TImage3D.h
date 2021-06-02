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

#ifndef WM4TIMAGE3D_H
#define WM4TIMAGE3D_H

#include "Wm4ImagicsLIB.h"
#include "Wm4TImage.h"

namespace Wm4
{

template <class T>
class TImage3D : public TImage<T>
{
public:
    // Construction and destruction.  TImage3D accepts responsibility for
    // deleting the input data array.
    TImage3D (int iXBound, int iYBound, int iZBound, T* atData = 0);
    TImage3D (const TImage3D& rkImage);
    TImage3D (const char* acFilename);

    // data access
    T& operator() (int iX, int iY, int iZ) const;

    // conversion between 3D coordinates and 1D indexing
    int GetIndex (int iX, int iY, int iZ) const;
    void GetCoordinates (int iIndex, int& riX, int& riY, int& riZ) const;

    // assignment
    TImage3D& operator= (const TImage3D& rkImage);
    TImage3D& operator= (T tValue);

protected:
    using TImage<T>::SetData;
    using TImage<T>::m_atData;
};

#include "Wm4TImage3D.inl"

}

#endif
