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

#ifndef WM4CONTROLLER_H
#define WM4CONTROLLER_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Object.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Controller : public Object
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // destruction (abstract base class)
    virtual ~Controller ();

    // the controlled object
    Object* GetObject () const;

    // animation update
    virtual bool Update (double dAppTime);

    // time management

    enum RepeatType
    {
        RT_CLAMP,
        RT_WRAP,
        RT_CYCLE,
        RT_QUANTITY
    };

    RepeatType Repeat;  // default = RT_CLAMP
    double MinTime;     // default = 0.0
    double MaxTime;     // default = 0.0
    double Phase;       // default = 0.0
    double Frequency;   // default = 1.0
    bool Active;        // default = true

protected:
    // construction (abstract base class)
    Controller ();

    // the controlled object
    friend class Object;
    virtual void SetObject (Object* pkObject);

    // Conversion from application time units to controller time units.
    // Derived classes may use this in their update routines.
    double GetControlTime (double dAppTime);

    // Regular pointer used for controlled object to avoid circular smart
    // pointers between controller and object.
    Object* m_pkObject;

    double m_dLastAppTime;

private:
    static const char* ms_aacRepeatType[RT_QUANTITY];
};

WM4_REGISTER_STREAM(Controller);
typedef Pointer<Controller> ControllerPtr;
#include "Wm4Controller.inl"

}

#endif
