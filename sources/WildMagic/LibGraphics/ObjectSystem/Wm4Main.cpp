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
// Version: 4.0.2 (2007/08/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4Main.h"
#include "Wm4Object.h"
using namespace Wm4;

Main::InitializerArray* Main::ms_pkInitializers = 0;
Main::TerminatorArray* Main::ms_pkTerminators = 0;
int Main::ms_iStartObjects = 0;
int Main::ms_iFinalObjects = 0;
Catalog<Image>* Main::ms_pkImageCatalog = 0;
Catalog<Texture>* Main::ms_pkTextureCatalog = 0;
Catalog<VertexProgram>* Main::ms_pkVertexProgramCatalog = 0;
Catalog<PixelProgram>* Main::ms_pkPixelProgramCatalog = 0;

//----------------------------------------------------------------------------
void Main::AddInitializer (Initializer oInitialize)
{
    if (!ms_pkInitializers)
    {
        ms_pkInitializers = WM4_NEW InitializerArray;
    }

    ms_pkInitializers->push_back(oInitialize);
}
//----------------------------------------------------------------------------
void Main::Initialize ()
{
    bool bCountIsCorrect = true;

    // objects should not be created pre-main
    if (Object::InUse)
    {
        bCountIsCorrect = false;
        Object::PrintInUse("AppLog.txt",
            "Objects were created before pre-main initialization");
    }
    assert(bCountIsCorrect);

    ms_pkImageCatalog = WM4_NEW Catalog<Image>("Main");
    Catalog<Image>::SetActive(ms_pkImageCatalog);

    ms_pkTextureCatalog = WM4_NEW Catalog<Texture>("Main");
    Catalog<Texture>::SetActive(ms_pkTextureCatalog);

    ms_pkVertexProgramCatalog = WM4_NEW Catalog<VertexProgram>("Main");
    Catalog<VertexProgram>::SetActive(ms_pkVertexProgramCatalog);

    ms_pkPixelProgramCatalog = WM4_NEW Catalog<PixelProgram>("Main");
    Catalog<PixelProgram>::SetActive(ms_pkPixelProgramCatalog);

    if (ms_pkInitializers)
    {
        for (int i = 0; i < (int)ms_pkInitializers->size(); i++)
        {
            (*ms_pkInitializers)[i]();
        }
    }

    WM4_DELETE ms_pkInitializers;
    ms_pkInitializers = 0;

    // number of objects created during initialization
    if (Object::InUse)
    {
        ms_iStartObjects = Object::InUse->GetQuantity();
    }
    else
    {
        ms_iStartObjects = 0;
    }
}
//----------------------------------------------------------------------------
void Main::AddTerminator (Terminator oTerminate)
{
    if (!ms_pkTerminators)
    {
        ms_pkTerminators = WM4_NEW TerminatorArray;
    }

    ms_pkTerminators->push_back(oTerminate);
}
//----------------------------------------------------------------------------
void Main::Terminate ()
{
    bool bCountIsCorrect = true;

    // all objects created during the application should be deleted by now
    if (Object::InUse)
    {
        ms_iFinalObjects = Object::InUse->GetQuantity();
    }
    else
    {
        ms_iFinalObjects = 0;
    }

    if (ms_iStartObjects != ms_iFinalObjects)
    {
        bCountIsCorrect = false;
        Object::PrintInUse("AppLog.txt",
            "Not all objects were deleted before post-main termination");
    }

    if (ms_pkTerminators)
    {
        for (int i = 0; i < (int)ms_pkTerminators->size(); i++)
        {
            (*ms_pkTerminators)[i]();
        }
    }

    WM4_DELETE ms_pkTerminators;
    ms_pkTerminators = 0;

    if (Catalog<PixelProgram>::GetActive() == ms_pkPixelProgramCatalog)
    {
        Catalog<PixelProgram>::SetActive(0);
    }
    WM4_DELETE ms_pkPixelProgramCatalog;

    if (Catalog<VertexProgram>::GetActive() == ms_pkVertexProgramCatalog)
    {
        Catalog<VertexProgram>::SetActive(0);
    }
    WM4_DELETE ms_pkVertexProgramCatalog;

    if (Catalog<Texture>::GetActive() == ms_pkTextureCatalog)
    {
        Catalog<Texture>::SetActive(0);
    }
    WM4_DELETE ms_pkTextureCatalog;

    if (Catalog<Image>::GetActive() == ms_pkImageCatalog)
    {
        Catalog<Image>::SetActive(0);
    }
    WM4_DELETE ms_pkImageCatalog;

    if (bCountIsCorrect)
    {
        // objects should not be deleted post-main
        if (Object::InUse)
        {
            ms_iFinalObjects = Object::InUse->GetQuantity();
        }
        else
        {
            ms_iFinalObjects = 0;
        }

        if (ms_iFinalObjects != 0)
        {
            bCountIsCorrect = false;
            Object::PrintInUse("AppLog.txt",
                "Objects were deleted after post-main termination");
        }
    }

    assert(bCountIsCorrect);

    // Now that the object leak detection system has completed its tasks,
    // delete the hash table to free up memory so that the debug memory
    // system will not flag it as a leak.
    WM4_DELETE Object::InUse;
    Object::InUse = 0;
}
//----------------------------------------------------------------------------
