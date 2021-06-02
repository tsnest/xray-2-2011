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

#include "Wm4GraphicsPCH.h"
#include "Wm4Polypoint.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Polypoint,Geometry);
WM4_IMPLEMENT_STREAM(Polypoint);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Polypoint,Geometry);

//----------------------------------------------------------------------------
Polypoint::Polypoint (VertexBuffer* pkVBuffer)
    :
    Geometry(pkVBuffer,0)
{
    Type = GT_POLYPOINT;

    int iVQuantity = VBuffer->GetVertexQuantity();
    m_iActiveQuantity = iVQuantity;

    IBuffer = WM4_NEW IndexBuffer(iVQuantity);
    int* aiIndex = IBuffer->GetData();
    for (int i = 0; i < iVQuantity; i++)
    {
        aiIndex[i] = i;
    }
}
//----------------------------------------------------------------------------
Polypoint::Polypoint ()
{
    Type = GT_POLYPOINT;
    m_iActiveQuantity = 0;
}
//----------------------------------------------------------------------------
Polypoint::~Polypoint ()
{
}
//----------------------------------------------------------------------------
void Polypoint::SetActiveQuantity (int iActiveQuantity)
{
    int iVQuantity = VBuffer->GetVertexQuantity();
    if (0 <= iActiveQuantity && iActiveQuantity <= iVQuantity)
    {
        m_iActiveQuantity = iActiveQuantity;
    }
    else
    {
        m_iActiveQuantity = iVQuantity;
    }

    IBuffer->SetIndexQuantity(m_iActiveQuantity);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Polypoint::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Geometry::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iActiveQuantity);

    WM4_END_DEBUG_STREAM_LOAD(Polypoint);
}
//----------------------------------------------------------------------------
void Polypoint::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Geometry::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Polypoint::Register (Stream& rkStream) const
{
    return Geometry::Register(rkStream);
}
//----------------------------------------------------------------------------
void Polypoint::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Geometry::Save(rkStream);

    // native data
    rkStream.Write(m_iActiveQuantity);

    WM4_END_DEBUG_STREAM_SAVE(Polypoint);
}
//----------------------------------------------------------------------------
int Polypoint::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Geometry::GetDiskUsed(rkVersion) +
        sizeof(m_iActiveQuantity);
}
//----------------------------------------------------------------------------
StringTree* Polypoint::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("active quantity =",m_iActiveQuantity));

    // children
    pkTree->Append(Geometry::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
