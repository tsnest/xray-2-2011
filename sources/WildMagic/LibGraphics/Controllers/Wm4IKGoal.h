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

#ifndef WM4IKGOAL_H
#define WM4IKGOAL_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Spatial.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM IKGoal : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    IKGoal (Spatial* pkTarget, Spatial* pkEffector, float fWeight);

    SpatialPtr GetTarget ();
    SpatialPtr GetEffector ();
    float Weight;  // default = 1.0f

    Vector3f GetTargetPosition () const;
    Vector3f GetEffectorPosition () const;

protected:
    IKGoal ();

    SpatialPtr m_spkTarget;
    SpatialPtr m_spkEffector;
};

WM4_REGISTER_STREAM(IKGoal);
typedef Pointer<IKGoal> IKGoalPtr;
#include "Wm4IKGoal.inl"

}

#endif
