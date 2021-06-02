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
// Version: 4.0.1 (2008/07/29)

#include "Wm4FoundationPCH.h"
#include "Wm4Quaternion.h"

namespace Wm4
{
template<> const Quaternion<float>
    Quaternion<float>::IDENTITY(1.0f,0.0f,0.0f,0.0f);
template<> const Quaternion<float>
    Quaternion<float>::ZERO(0.0f,0.0f,0.0f,0.0f);
template<> int Quaternion<float>::ms_iNext[3] = { 1, 2, 0 };
template<> float Quaternion<float>::ms_fTolerance = 1e-06f;
template<> float Quaternion<float>::ms_fRootTwo = (float)sqrt(2.0);
template<> float Quaternion<float>::ms_fRootHalf = (float)sqrt(0.5);

template<> const Quaternion<double>
    Quaternion<double>::IDENTITY(1.0,0.0,0.0,0.0);
template<> const Quaternion<double>
    Quaternion<double>::ZERO(0.0,0.0,0.0,0.0);
template<> int Quaternion<double>::ms_iNext[3] = { 1, 2, 0 };
template<> double Quaternion<double>::ms_fTolerance = 1e-08;
template<> double Quaternion<double>::ms_fRootTwo = sqrt(2.0);
template<> double Quaternion<double>::ms_fRootHalf = sqrt(0.5);
}
