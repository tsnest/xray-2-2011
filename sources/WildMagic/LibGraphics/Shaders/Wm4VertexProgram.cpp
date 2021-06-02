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
// Version: 4.0.3 (2008/04/13)

#include "Wm4GraphicsPCH.h"
#include "Wm4VertexProgram.h"
#include "Wm4Catalog.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,VertexProgram,Program);
WM4_IMPLEMENT_STREAM(VertexProgram);
WM4_IMPLEMENT_DEFAULT_NAME_ID(VertexProgram,Program);

//----------------------------------------------------------------------------
VertexProgram::VertexProgram ()
{
}
//----------------------------------------------------------------------------
VertexProgram::VertexProgram (const std::string& rkProgramName)
{
    std::string kFilename = std::string("v_") + rkProgramName +
        std::string(".") + RendererType + std::string(".wmsp");
    bool bLoaded = Program::Load(kFilename,this);
    assert(bLoaded);
    (void)bLoaded;  // Avoid warning in release builds.
    SetName(rkProgramName.c_str());

    // Verify that the vertex program does not require more resources than the
    // renderer can support.
    assert(m_kInputAttributes.GetMaxColors() <= Renderer::GetMaxColors());
    assert(m_kInputAttributes.GetMaxTCoords() <= Renderer::GetMaxTCoords());
    assert(m_kOutputAttributes.GetMaxColors() <= Renderer::GetMaxColors());
    assert(m_kOutputAttributes.GetMaxTCoords() <= Renderer::GetMaxTCoords());
    assert(
        (int)m_kSamplerInformation.size() <= Renderer::GetMaxVShaderImages());

    Catalog<VertexProgram>::GetActive()->Insert(this);
}
//----------------------------------------------------------------------------
VertexProgram::~VertexProgram ()
{
    Catalog<VertexProgram>::RemoveAll(this);
}
//----------------------------------------------------------------------------
VertexProgram* VertexProgram::Load (const std::string& rkProgramName)
{
    return WM4_NEW VertexProgram(rkProgramName);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void VertexProgram::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Program::Load(rkStream,pkLink);

    std::string kFilename = std::string("v_") + GetName() +
        std::string(".") + RendererType + std::string(".wmsp");
    bool bLoaded = Program::Load(kFilename,this);
    assert(bLoaded);
    (void)bLoaded;  // Avoid warning in release builds.

    Catalog<VertexProgram>::GetActive()->Insert(this);

    WM4_END_DEBUG_STREAM_LOAD(VertexProgram);
}
//----------------------------------------------------------------------------
void VertexProgram::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Program::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool VertexProgram::Register (Stream& rkStream) const
{
    return Program::Register(rkStream);
}
//----------------------------------------------------------------------------
void VertexProgram::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Program::Save(rkStream);

    WM4_END_DEBUG_STREAM_SAVE(VertexProgram);
}
//----------------------------------------------------------------------------
int VertexProgram::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Program::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* VertexProgram::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Program::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
