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
#include "Wm4ColorRGBA.h"

namespace Wm4
{

const ColorRGBA ColorRGBA::BLACK(0.0f,0.0f,0.0f,1.0f);
const ColorRGBA ColorRGBA::WHITE(1.0f,1.0f,1.0f,1.0f);
const ColorRGBA ColorRGBA::INVALID(-1.0f,-1.0f,-1.0f,-1.0f);

//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA ()
{
    m_afTuple[0] = 0.0f;
    m_afTuple[1] = 0.0f;
    m_afTuple[2] = 0.0f;
    m_afTuple[3] = 0.0f;
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (float fR, float fG, float fB, float fA)
{
    m_afTuple[0] = fR;
    m_afTuple[1] = fG;
    m_afTuple[2] = fB;
    m_afTuple[3] = fA;
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (const float* afTuple)
{
    m_afTuple[0] = afTuple[0];
    m_afTuple[1] = afTuple[1];
    m_afTuple[2] = afTuple[2];
    m_afTuple[3] = afTuple[3];
}
//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA (const ColorRGBA& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
    m_afTuple[3] = rkC.m_afTuple[3];
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator= (const ColorRGBA& rkC)
{
    m_afTuple[0] = rkC.m_afTuple[0];
    m_afTuple[1] = rkC.m_afTuple[1];
    m_afTuple[2] = rkC.m_afTuple[2];
    m_afTuple[3] = rkC.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator== (const ColorRGBA& rkC) const
{
    return
        m_afTuple[0] == rkC.m_afTuple[0] &&
        m_afTuple[1] == rkC.m_afTuple[1] &&
        m_afTuple[2] == rkC.m_afTuple[2] &&
        m_afTuple[3] == rkC.m_afTuple[3];
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator!= (const ColorRGBA& rkC) const
{
    return
        m_afTuple[0] != rkC.m_afTuple[0] ||
        m_afTuple[1] != rkC.m_afTuple[1] ||
        m_afTuple[2] != rkC.m_afTuple[2] ||
        m_afTuple[3] != rkC.m_afTuple[3];
}
//----------------------------------------------------------------------------
int ColorRGBA::CompareArrays (const ColorRGBA& rkC) const
{
    return memcmp(m_afTuple,rkC.m_afTuple,4*sizeof(float));
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator< (const ColorRGBA& rkC) const
{
    return CompareArrays(rkC) < 0;
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator<= (const ColorRGBA& rkC) const
{
    return CompareArrays(rkC) <= 0;
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator> (const ColorRGBA& rkC) const
{
    return CompareArrays(rkC) > 0;
}
//----------------------------------------------------------------------------
bool ColorRGBA::operator>= (const ColorRGBA& rkC) const
{
    return CompareArrays(rkC) >= 0;
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator+ (const ColorRGBA& rkC) const
{
    return ColorRGBA(
        m_afTuple[0] + rkC.m_afTuple[0],
        m_afTuple[1] + rkC.m_afTuple[1],
        m_afTuple[2] + rkC.m_afTuple[2],
        m_afTuple[3] + rkC.m_afTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator- (const ColorRGBA& rkC) const
{
    return ColorRGBA(
        m_afTuple[0] - rkC.m_afTuple[0],
        m_afTuple[1] - rkC.m_afTuple[1],
        m_afTuple[2] - rkC.m_afTuple[2],
        m_afTuple[3] - rkC.m_afTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator* (const ColorRGBA& rkC) const
{
    return ColorRGBA(
        m_afTuple[0]*rkC.m_afTuple[0],
        m_afTuple[1]*rkC.m_afTuple[1],
        m_afTuple[2]*rkC.m_afTuple[2],
        m_afTuple[3]*rkC.m_afTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA ColorRGBA::operator* (float fScalar) const
{
    return ColorRGBA(
        fScalar*m_afTuple[0],
        fScalar*m_afTuple[1],
        fScalar*m_afTuple[2],
        fScalar*m_afTuple[3]);
}
//----------------------------------------------------------------------------
ColorRGBA operator* (float fScalar, const ColorRGBA& rkC)
{
    return ColorRGBA(
        fScalar*rkC[0],
        fScalar*rkC[1],
        fScalar*rkC[2],
        fScalar*rkC[3]);
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator+= (const ColorRGBA& rkC)
{
    m_afTuple[0] += rkC.m_afTuple[0];
    m_afTuple[1] += rkC.m_afTuple[1];
    m_afTuple[2] += rkC.m_afTuple[2];
    m_afTuple[3] += rkC.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator-= (const ColorRGBA& rkC)
{
    m_afTuple[0] -= rkC.m_afTuple[0];
    m_afTuple[1] -= rkC.m_afTuple[1];
    m_afTuple[2] -= rkC.m_afTuple[2];
    m_afTuple[3] -= rkC.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator*= (const ColorRGBA& rkC)
{
    m_afTuple[0] *= rkC.m_afTuple[0];
    m_afTuple[1] *= rkC.m_afTuple[1];
    m_afTuple[2] *= rkC.m_afTuple[2];
    m_afTuple[3] *= rkC.m_afTuple[3];
    return *this;
}
//----------------------------------------------------------------------------
ColorRGBA& ColorRGBA::operator*= (float fScalar)
{
    m_afTuple[0] *= fScalar;
    m_afTuple[1] *= fScalar;
    m_afTuple[2] *= fScalar;
    m_afTuple[3] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
void ColorRGBA::Clamp ()
{
    for (int i = 0; i < 4; i++)
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
void ColorRGBA::ScaleByMax ()
{
    // Use max of color channels, not alpha channel.
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

    // clamp alpha to [0,1]
    if (m_afTuple[3] > 1.0f)
    {
        m_afTuple[3] = 1.0f;
    }
    else if (m_afTuple[3] < 0.0f)
    {
        m_afTuple[3] = 0.0f;
    }
}
//----------------------------------------------------------------------------
}
