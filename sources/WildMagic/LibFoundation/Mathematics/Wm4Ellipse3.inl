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
// Version: 4.7.0 (2008/08/17)

//----------------------------------------------------------------------------
template <class Real>
Ellipse3<Real>::Ellipse3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Ellipse3<Real>::Ellipse3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkNormal, const Vector3<Real>& rkMajor,
    const Vector3<Real>& rkMinor, Real fMajorLength, Real fMinorLength)
    :
    Center(rkCenter),
    Normal(rkNormal),
    Major(rkMajor),
    Minor(rkMinor)
{
    MajorLength = fMajorLength;
    MinorLength = fMinorLength;
}
//----------------------------------------------------------------------------
