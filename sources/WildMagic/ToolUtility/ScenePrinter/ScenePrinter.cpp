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
// Version: 4.0.1 (2008/12/13)

#include "ScenePrinter.h"

WM4_WINDOW_APPLICATION(ScenePrinter);

// Usage:  ScenePrinter myfile.wmof
// Output:  myfile.txt

//----------------------------------------------------------------------------
ScenePrinter::ScenePrinter ()
    :
    WindowApplication3("ScenePrinter",0,0,640,480,ColorRGBA::WHITE)
{
}
//----------------------------------------------------------------------------
bool ScenePrinter::OnInitialize ()
{
    if (WindowApplication3::OnInitialize())
    {
        char* acFilename = 0;
        if (!TheCommand->Filename(acFilename))
        {
            // There must be an input WMOF file.
            return false;
        }

        int iLength = (int)strlen(acFilename);
        if (iLength <= 5)
        {
            // The input file name is not large enough to contain the .wmof
            // extension.
            WM4_DELETE[] acFilename;
            return false;
        }

        if (acFilename[iLength-5] != '.'
        ||  acFilename[iLength-4] != 'w'
        ||  acFilename[iLength-3] != 'm'
        ||  acFilename[iLength-2] != 'o'
        ||  acFilename[iLength-1] != 'f')
        {
            // The input file does not end in the .wmof extension.
            WM4_DELETE[] acFilename;
            return false;
        }

        // Load the scene graphs.
        Stream kStream;
        if (!kStream.Load(acFilename))
        {
            // Failed to load the file.
            WM4_DELETE[] acFilename;
            return false;
        }

        // Replace the .wmof extension by the .txt extension.
        acFilename[iLength-4] = 't';
        acFilename[iLength-3] = 'x';
        acFilename[iLength-2] = 't';
        acFilename[iLength-1] = 0;

        // Update the scenes to ensure all the world data and render state
        // is current.
        for (int i = 0; i < kStream.GetObjectCount(); i++)
        {
            Spatial* pkSpatial = DynamicCast<Spatial>(kStream.GetObjectAt(i));
            if (pkSpatial)
            {
                pkSpatial->UpdateGS();
                pkSpatial->UpdateRS();
            }
        }

        if (!kStream.SaveText(acFilename))
        {
            // Failed to save the file.
            WM4_DELETE[] acFilename;
            return false;
        }
    }

    // The window is not needed, so just inform the application layer to
    // terminate.  We need to do things this way because the renderer must
    // be created in order to load shader programs.
    return false;
}
//----------------------------------------------------------------------------
