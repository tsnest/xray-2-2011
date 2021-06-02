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
LinComp3<Real>::LinComp3 ()
    :
    Origin(Vector3<Real>::ZERO),
    Direction(Vector3<Real>::ZERO)
{
}
//----------------------------------------------------------------------------
template <class Real>
LinComp3<Real>::LinComp3 (const Vector3<Real>& rkOrigin,
    const Vector3<Real>& rkDirection, Real fMin, Real fMax)
    :
    Origin(rkOrigin),
    Direction(rkDirection)
{
    SetInterval(fMin,fMax);
}
//----------------------------------------------------------------------------
template <class Real>
LinComp3<Real>::LinComp3 (const LinComp3& rkComponent)
    :
    LinComp<Real>(rkComponent),
    Origin(rkComponent.Origin),
    Direction(rkComponent.Direction)
{
}
//----------------------------------------------------------------------------
template <class Real>
LinComp3<Real>& LinComp3<Real>::operator= (const LinComp3& rkComponent)
{
    LinComp<Real>::operator=(rkComponent);
    Origin = rkComponent.Origin;
    Direction = rkComponent.Direction;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
void LinComp3<Real>::MakeCanonical ()
{
    switch (m_iType)
    {
    case CT_LINE:
        // nothing to do
        break;
    case CT_RAY:
        if (m_fMax == Math<Real>::MAX_REAL)
        {
            if (m_fMin != (Real)0.0)
            {
                Origin += Direction*m_fMin;
                m_fMin = (Real)0.0;
            }
        }
        else  // m_fMin == -Math<Real>::MAX_REAL
        {
            Origin += Direction*m_fMax;
            Direction = -Direction;
            m_fMin = (Real)0.0;
            m_fMax = Math<Real>::MAX_REAL;
        }
        break;
    case CT_SEGMENT:
        if (m_fMin != -m_fMax)
        {
            Origin += Direction*(((Real)0.5)*(m_fMin+m_fMax));
            m_fMax = ((Real)0.5)*(m_fMax-m_fMin);
            m_fMin = -m_fMax;
        }
        break;
    case CT_POINT:
        if (m_fMin != (Real)0.0)
        {
            Origin += Direction*m_fMin;
            m_fMin = (Real)0.0;
            m_fMax = (Real)0.0;
        }
        break;
    case CT_EMPTY:
        m_fMin = Math<Real>::MAX_REAL;
        m_fMax = -Math<Real>::MAX_REAL;
        break;
    }
}
//----------------------------------------------------------------------------
