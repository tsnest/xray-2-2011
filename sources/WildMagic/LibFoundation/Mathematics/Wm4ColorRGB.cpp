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
// Version: 4.0.1 (2007/09/26)

#include "Wm4FoundationPCH.h"
#include "Wm4ColorRGB.h"

namespace Wm4
{

const ColorRGB ColorRGB::BLACK(0.0f,0.0f,0.0f);
const ColorRGB ColorRGB::WHITE(1.0f,1.0f,1.0f);
const ColorRGB ColorRGB::INVALID(-1.0f,-1.0f,-1.0f);

//----------------------------------------------------------------------------
ColorRGB::ColorRGB ()
{
    m_afTuple[0] = 0.0f;
    m_afTuple[1] = 0.0f;
    m_afTuple[2] = 0.0f;
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (float fR, float fG, float fB)
{
    m_afTuple[0] = fR;
    m_afTuple[1] = fG;
    m_afTuple[2] = fB;
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (const float* afTuple)
{
    m_afTuple[0] = afTuple[0];
    m_afTuple[1] = afTuple[1];
    m_afTuple[2] = afTuple[2];
}
//----------------------------------------------------------------------------
ColorRGB::ColorRGB (const ColorRGB& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator= (const ColorRGB& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator== (const ColorRGB& rkC) const
{
    return
        m_afTuple[0] == rkC.m_afTuple[0] &&
        m_afTuple[1] == rkC.m_afTuple[1] &&
        m_afTuple[2] == rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
bool ColorRGB::operator!= (const ColorRGB& rkC) const
{
    return
        m_afTuple[0] != rkC.m_afTuple[0] ||
        m_afTuple[1] != rkC.m_afTuple[1] ||
        m_afTuple[2] != rkC.m_afTuple[2];
}
//----------------------------------------------------------------------------
int ColorRGB::CompareArrays (const ColorRGB& rkC) const
{
    return memcmp(m_afTuple,rkC.m_afTuple,3*sizeof(float));
}
//----------------------------------------------------------------------------
bool ColorRGB::operator< (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) < 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator<= (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) <= 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator> (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) > 0;
}
//----------------------------------------------------------------------------
bool ColorRGB::operator>= (const ColorRGB& rkC) const
{
    return CompareArrays(rkC) >= 0;
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator+ (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0] + rkC.m_afTuple[0],
        m_afTuple[1] + rkC.m_afTuple[1],
        m_afTuple[2] + rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator- (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0] - rkC.m_afTuple[0],
        m_afTuple[1] - rkC.m_afTuple[1],
        m_afTuple[2] - rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator* (const ColorRGB& rkC) const
{
    return ColorRGB(
        m_afTuple[0]*rkC.m_afTuple[0],
        m_afTuple[1]*rkC.m_afTuple[1],
        m_afTuple[2]*rkC.m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB ColorRGB::operator* (float fScalar) const
{
    return ColorRGB(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2]);
}
//----------------------------------------------------------------------------
ColorRGB operator* (float fScalar, const ColorRGB& rkC)
{
    return ColorRGB(fScalar*rkC[0],fScalar*rkC[1],fScalar*rkC[2]);
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator+= (const ColorRGB& rkC)
{
    m_afTuple[0] += rkC.m_afTuple[0];
    m_afTuple[1] += rkC.m_afTuple[1];
    m_afTuple[2] += rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator-= (const ColorRGB& rkC)
{
    m_afTuple[0] -= rkC.m_afTuple[0];
    m_afTuple[1] -= rkC.m_afTuple[1];
    m_afTuple[2] -= rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator*= (const ColorRGB& rkC)
{
    m_afTuple[0] *= rkC.m_afTuple[0];
    m_afTuple[1] *= rkC.m_afTuple[1];
    m_afTuple[2] *= rkC.m_afTuple[2];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGB& ColorRGB::operator*= (float fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
void ColorRGB::Clamp ()
{
    for (int i = 0; i < 3; i++)
    {
        if (m_afTuple[i] > 1.0f)
        {
            m_afTuple[i] = 1.0f;
        }
        else if (m_afTuple[i] < 0.0f)
        {
            m_afTuple[i] = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
void ColorRGB::ScaleByMax ()
{
    float fMax = m_afTuple[0];
    if (m_afTuple[1] > fMax)
    {
        fMax = m_afTuple[1];
    }
    if (m_afTuple[2] > fMax)
    {
        fMax = m_afTuple[2];
    }

    if (fMax > 1.0f)
    {
        float fInvMax = 1.0f/fMax;
        for (int i = 0; i < 3; i++)
        {
            m_afTuple[i] *= fInvMax;
        }
    }
}
//----------------------------------------------------------------------------
}
