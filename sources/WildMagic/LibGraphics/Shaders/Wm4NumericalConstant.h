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

#ifndef WM4NUMERICALCONSTANT_H
#define WM4NUMERICALCONSTANT_H

#include "Wm4GraphicsLIB.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM NumericalConstant
{
public:
    // Construction and destruction.  The register must be nonnegative.  A
    // numerical constant corresponds to four floating-point numbers.
    NumericalConstant (int iRegister, float afData[4]);
    ~NumericalConstant ();

    // Member access.  The renderer will use these to pass the information to
    // the graphics API (specifically, to DirectX).
    int GetRegister () const;
    float* GetData () const;

private:
    int m_iRegister;
    float m_afData[4];
};

#include "Wm4NumericalConstant.inl"

}

#endif
