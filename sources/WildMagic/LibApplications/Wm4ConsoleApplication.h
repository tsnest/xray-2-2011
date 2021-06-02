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

#ifndef WM4CONSOLEAPPLICATION_H
#define WM4CONSOLEAPPLICATION_H

#include "Wm4Application.h"

namespace Wm4
{

class ConsoleApplication : public Application
{
public:
    ConsoleApplication ();
    virtual ~ConsoleApplication ();

    // Entry point to be implemented by the application.  The return value
    // is an exit code, if desired.
    virtual int Main (int iQuantity, char** apcArgument) = 0;

protected:
    // The hookup to the 'main' entry point into the executable.
    static int Run (int iQuantity, char** apcArgument);
};

}

#endif
