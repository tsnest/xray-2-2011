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

#include "Wm4GraphicsPCH.h"
#include "Wm4SamplerInformation.h"
using namespace Wm4;

//----------------------------------------------------------------------------
SamplerInformation::SamplerInformation (const std::string& rkName, Type eType,
    int iTextureUnit)
    :
    m_kName(rkName)
{
    m_eType = eType;
    m_iTextureUnit = iTextureUnit;

    switch (m_eType)
    {
    case SAMPLER_1D:
        m_iDimension = 1;
        break;
    case SAMPLER_2D:
        m_iDimension = 2;
        break;
    case SAMPLER_3D:
        m_iDimension = 3;
        break;
    case SAMPLER_CUBE:
        m_iDimension = 2;
        break;
    case SAMPLER_PROJ:
        m_iDimension = 2;
        break;
    default:
        assert(false);
        m_iDimension = 0;
        break;
    }
}
//----------------------------------------------------------------------------
SamplerInformation::~SamplerInformation ()
{
}
//----------------------------------------------------------------------------
