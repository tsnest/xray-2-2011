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

#ifndef WM4USERCONSTANT_H
#define WM4USERCONSTANT_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM UserConstant
{
public:
    // Construction and destruction.  The base register must be nonnegative.
    // The register quantity must be positive.  Each register represents four
    // floating-point values.
    UserConstant (const std::string& rkName, int iBaseRegister,
        int iRegisterQuantity);
    ~UserConstant ();

    // Member access.  The renderer will use these to pass the information to
    // the graphics API.
    const std::string& GetName () const;
    int GetBaseRegister () const;
    int GetRegisterQuantity () const;
    float* GetData () const;

    // The Shader base class provides storage for the user constants and
    // will set the float pointer to this storage when the shader program is
    // loaded.  However, Shader-derived classes may provide their own
    // storage and set the float pointer accordingly.  Such derived classes
    // are responsible for deallocating the storage if it was dynamically
    // allocated.
    void SetDataSource (float* afData);

private:
    std::string m_kName;
    int m_iBaseRegister;
    int m_iRegisterQuantity;
    float* m_afData;
};

#include "Wm4UserConstant.inl"

}

#endif
