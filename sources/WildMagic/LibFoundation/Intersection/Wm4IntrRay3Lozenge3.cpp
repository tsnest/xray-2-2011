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
#include "Wm4IntrRay3Lozenge3.h"
#include "Wm4DistRay3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrRay3Lozenge3<Real>::IntrRay3Lozenge3 (const Ray3<Real>& rkRay,
    const Lozenge3<Real>& rkLozenge)
    :
    m_pkRay(&rkRay),
    m_pkLozenge(&rkLozenge)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Ray3<Real>& IntrRay3Lozenge3<Real>::GetRay () const
{
    return *m_pkRay;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrRay3Lozenge3<Real>::GetLozenge () const
{
    return *m_pkLozenge;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrRay3Lozenge3<Real>::Test ()
{
    Real fDist = DistRay3Rectangle3<Real>(*m_pkRay,
        m_pkLozenge->Rectangle).Get();

    return fDist <= m_pkLozenge->Radius;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrRay3Lozenge3<float>;

template WM4_FOUNDATION_ITEM
class IntrRay3Lozenge3<double>;
//----------------------------------------------------------------------------
}
