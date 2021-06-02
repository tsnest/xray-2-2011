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

#ifndef WM4CURVATUREFLOW2_H
#define WM4CURVATUREFLOW2_H

#include "Wm4ImagicsLIB.h"
#include "Wm4PdeFilter2.h"

namespace Wm4
{

class WM4_IMAGICS_ITEM CurvatureFlow2 : public PdeFilter2
{
public:
    CurvatureFlow2 (int iXBound, int iYBound, float fXSpacing,
        float fYSpacing, const float* afData, const bool* abMask,
        float fBorderValue, ScaleType eType);

    virtual ~CurvatureFlow2 ();

protected:
    virtual void OnUpdate (int iX, int iY);
};

}

#endif
