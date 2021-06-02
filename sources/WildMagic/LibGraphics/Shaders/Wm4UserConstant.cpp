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
#include "Wm4UserConstant.h"
using namespace Wm4;

//----------------------------------------------------------------------------
UserConstant::UserConstant (const std::string& rkName, int iBaseRegister,
    int iRegisterQuantity)
    :
    m_kName(rkName)
{
    assert(iBaseRegister >= 0);
    assert(iRegisterQuantity > 0);

    m_iBaseRegister = iBaseRegister;
    m_iRegisterQuantity = iRegisterQuantity;

    // To be set later in the effect creation.
    m_afData = 0;
}
//----------------------------------------------------------------------------
UserConstant::~UserConstant ()
{
    // The memory pointed to by m_afData is not owned by UserConstant, so
    // no deletion occurs here.
}
//----------------------------------------------------------------------------
void UserConstant::SetDataSource (float* afData)
{
    assert(afData);

    m_afData = afData;
}
//----------------------------------------------------------------------------
