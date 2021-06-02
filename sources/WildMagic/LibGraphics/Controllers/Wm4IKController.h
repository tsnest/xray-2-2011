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

#ifndef WM4IKCONTROLLER_H
#define WM4IKCONTROLLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Controller.h"
#include "Wm4IKGoal.h"
#include "Wm4IKJoint.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM IKController : public Controller
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // Construction and destruction.  IKController assumes responsibility for
    // the input arrays and will delete them.  They should be dynamically
    // allocated.
    IKController (int iJointQuantity, IKJointPtr* aspkJoint,
        int iGoalQuantity, IKGoalPtr* aspkGoal);
    virtual ~IKController ();

    int Iterations;       // default = 128
    bool OrderEndToRoot;  // default = true

    virtual bool Update (double dAppTime);

protected:
    IKController ();

    int m_iJointQuantity;
    IKJointPtr* m_aspkJoint;

    int m_iGoalQuantity;
    IKGoalPtr* m_aspkGoal;
};

WM4_REGISTER_STREAM(IKController);
typedef Pointer<IKController> IKControllerPtr;

}

#endif
