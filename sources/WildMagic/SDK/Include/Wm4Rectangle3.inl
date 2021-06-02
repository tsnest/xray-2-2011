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

//----------------------------------------------------------------------------
template <class Real>
Rectangle3<Real>::Rectangle3 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>* akAxis, const Real* afExtent)
    :
    Center(rkCenter)
{
    for (int i = 0; i < 2; i++)
    {
        Axis[i] = akAxis[i];
        Extent[i] = afExtent[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Rectangle3<Real>::Rectangle3 (const Vector3<Real>& rkCenter,
    const Vector3<Real>& rkAxis0, const Vector3<Real>& rkAxis1,
    Real fExtent0, Real fExtent1)
    :
    Center(rkCenter)
{
    Axis[0] = rkAxis0;
    Axis[1] = rkAxis1;
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
}
//----------------------------------------------------------------------------
template <class Real>
void Rectangle3<Real>::ComputeVertices (Vector3<Real> akVertex[4]) const
{
    Vector3<Real> akEAxis[2] =
    {
        Extent[0]*Axis[0],
        Extent[1]*Axis[1]
    };

    akVertex[0] = Center - akEAxis[0] - akEAxis[1];
    akVertex[1] = Center + akEAxis[0] - akEAxis[1];
    akVertex[2] = Center + akEAxis[0] + akEAxis[1];
    akVertex[3] = Center - akEAxis[0] + akEAxis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetPPCorner () const
{
    return Center + Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetPMCorner () const
{
    return Center + Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetMPCorner () const
{
    return Center - Extent[0]*Axis[0] + Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Rectangle3<Real>::GetMMCorner () const
{
    return Center - Extent[0]*Axis[0] - Extent[1]*Axis[1];
}
//----------------------------------------------------------------------------
