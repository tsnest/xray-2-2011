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
// Version: 4.0.0 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4PdeFilter.h"
using namespace Wm4;

//----------------------------------------------------------------------------
PdeFilter::PdeFilter (int iQuantity, const float* afData, float fBorderValue,
    ScaleType eScaleType)
{
    assert(afData);

    m_iQuantity = iQuantity;
    m_fBorderValue = fBorderValue;
    m_eScaleType = eScaleType;
    m_fTimeStep = 0.0f;

    float fMax = afData[0];
    m_fMin = fMax;
    for (int i = 1; i < m_iQuantity; i++)
    {
        float fValue = afData[i];
        if (fValue < m_fMin)
        {
            m_fMin = fValue;
        }
        else if (fValue > fMax)
        {
            fMax = fValue;
        }
    }

    if (m_fMin != fMax)
    {
        switch (m_eScaleType)
        {
        case ST_NONE:
            m_fOffset = 0.0f;
            m_fScale = 1.0f;
            break;
        case ST_UNIT:
            m_fOffset = 0.0f;
            m_fScale = 1.0f/(fMax - m_fMin);
            break;
        case ST_SYMMETRIC:
            m_fOffset = -1.0f;
            m_fScale = 2.0f/(fMax - m_fMin);
            break;
        case ST_PRESERVE_ZERO:
            m_fOffset = 0.0f;
            m_fScale = (fMax >= -m_fMin ? 1.0f/fMax : -1.0f/m_fMin);
            m_fMin = 0.0f;
            break;
        }
    }
    else
    {
        m_fOffset = 0.0f;
        m_fScale = 1.0f;
    }
}
//----------------------------------------------------------------------------
PdeFilter::~PdeFilter ()
{
}
//----------------------------------------------------------------------------
void PdeFilter::Update ()
{
    OnPreUpdate();
    OnUpdate();
    OnPostUpdate();
}
//----------------------------------------------------------------------------
