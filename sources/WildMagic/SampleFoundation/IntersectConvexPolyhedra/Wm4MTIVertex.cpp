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
// Version: 4.7.0 (2008/09/15)

#include "Wm4MTIVertex.h"
using namespace Wm4;

//----------------------------------------------------------------------------
MTIVertex::MTIVertex (int iLabel)
{
    m_iLabel = iLabel;
}
//----------------------------------------------------------------------------
int MTIVertex::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
bool MTIVertex::operator< (const MTIVertex& rkV) const
{
    return m_iLabel < rkV.m_iLabel;
}
//----------------------------------------------------------------------------
bool MTIVertex::operator== (const MTIVertex& rkV) const
{
    return m_iLabel == rkV.m_iLabel;
}
//----------------------------------------------------------------------------
bool MTIVertex::operator!= (const MTIVertex& rkV) const
{
    return m_iLabel != rkV.m_iLabel;
}
//----------------------------------------------------------------------------
