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

#ifndef BLOODCELLCONTROLLER_H
#define BLOODCELLCONTROLLER_H

#include "Wm4ParticleController.h"

namespace Wm4
{

class BloodCellController : public ParticleController
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    BloodCellController ();
protected:
    virtual void UpdatePointMotion (float fCtrlTime);
};

WM4_REGISTER_STREAM(BloodCellController);

}

#endif
