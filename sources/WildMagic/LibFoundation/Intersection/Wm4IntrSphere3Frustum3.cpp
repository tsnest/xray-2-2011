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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrSphere3Frustum3.h"
#include "Wm4DistVector3Frustum3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrSphere3Frustum3<Real>::IntrSphere3Frustum3 (const Sphere3<Real>& rkSphere,
    const Frustum3<Real>& rkFrustum)
    :
    m_pkSphere(&rkSphere),
    m_pkFrustum(&rkFrustum)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Sphere3<Real>& IntrSphere3Frustum3<Real>::GetSphere () const
{
    return *m_pkSphere;
}
//----------------------------------------------------------------------------
template <class Real>
const Frustum3<Real>& IntrSphere3Frustum3<Real>::GetFrustum () const
{
    return *m_pkFrustum;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSphere3Frustum3<Real>::Test ()
{
    Real fDist = DistVector3Frustum3<Real>(m_pkSphere->Center,
        *m_pkFrustum).Get();
    return fDist <= m_pkSphere->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrSphere3Frustum3<float>;

template WM4_FOUNDATION_ITEM
class IntrSphere3Frustum3<double>;
//----------------------------------------------------------------------------
}
