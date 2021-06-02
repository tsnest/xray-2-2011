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
#include "Wm4Effect.h"
#include "Wm4Geometry.h"
#include "Wm4Renderer.h"
#include "Wm4VisibleSet.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Effect,Object);
WM4_IMPLEMENT_STREAM(Effect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Effect,Object);

//----------------------------------------------------------------------------
Effect::Effect ()
{
}
//----------------------------------------------------------------------------
Effect::~Effect ()
{
}
//----------------------------------------------------------------------------
void Effect::Draw (Renderer* pkRenderer, Spatial*, int iMin, int iMax,
    VisibleObject* akVisible)
{
    // The default drawing function for global effects.  Essentially, this is
    // a local effect applied to all the visible leaf geometry.
    for (int i = iMin; i <= iMax; i++)
    {
        if (akVisible[i].IsDrawable())
        {
            Geometry* pkGeometry = (Geometry*)akVisible[i].Object;
            pkGeometry->AttachEffect(this);
            pkRenderer->Draw(pkGeometry);
            pkGeometry->DetachEffect(this);
        }
    }
}
//----------------------------------------------------------------------------
void Effect::LoadResources (Renderer*, Geometry*)
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------
void Effect::ReleaseResources (Renderer*, Geometry*)
{
    // Stub for derived classes.
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Effect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Object::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(Effect);
}
//----------------------------------------------------------------------------
void Effect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Effect::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void Effect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Object::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(Effect);
}
//----------------------------------------------------------------------------
int Effect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* Effect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Object::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
