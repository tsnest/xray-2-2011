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
#include "Wm4IntrLozenge3Lozenge3.h"
#include "Wm4DistRectangle3Rectangle3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLozenge3Lozenge3<Real>::IntrLozenge3Lozenge3 (
    const Lozenge3<Real>& rkLozenge0, const Lozenge3<Real>& rkLozenge1)
    :
    m_pkLozenge0(&rkLozenge0),
    m_pkLozenge1(&rkLozenge1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge0 () const
{
    return *m_pkLozenge0;
}
//----------------------------------------------------------------------------
template <class Real>
const Lozenge3<Real>& IntrLozenge3Lozenge3<Real>::GetLozenge1 () const
{
    return *m_pkLozenge1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLozenge3Lozenge3<Real>::Test ()
{
    Real fDist = DistRectangle3Rectangle3<Real>(m_pkLozenge0->Rectangle,
        m_pkLozenge1->Rectangle).Get();
    Real fRSum = m_pkLozenge0->Radius + m_pkLozenge1->Radius;
    return fDist <= fRSum;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLozenge3Lozenge3<float>;

template WM4_FOUNDATION_ITEM
class IntrLozenge3Lozenge3<double>;
//----------------------------------------------------------------------------
}
