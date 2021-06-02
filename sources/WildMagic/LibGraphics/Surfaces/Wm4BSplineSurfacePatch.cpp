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
#include "Wm4BSplineSurfacePatch.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,BSplineSurfacePatch,SurfacePatch);
WM4_IMPLEMENT_STREAM(BSplineSurfacePatch);
WM4_IMPLEMENT_DEFAULT_NAME_ID(BSplineSurfacePatch,SurfacePatch);

//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch ()
    :
    SurfacePatch(0.0f,1.0f,0.0f,1.0f,true),
    m_iConstructor(0)
{
    m_pkPatch = 0;
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3f** aakCtrlPoint, int iUDegree, int iVDegree,
    bool bULoop, bool bVLoop, bool bUOpen, bool bVOpen)
    :
    SurfacePatch(0.0f,1.0f,0.0f,1.0f,true),
    m_iConstructor(1)
{
    m_pkPatch = WM4_NEW BSplineRectanglef(iNumUCtrlPoints,iNumVCtrlPoints,
        aakCtrlPoint,iUDegree,iVDegree,bULoop,bVLoop,bUOpen,bVOpen);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3f** aakCtrlPoint, int iUDegree, int iVDegree,
    bool bULoop, bool bVLoop, bool bUOpen, float* afVKnot)
    :
    SurfacePatch(0.0f,1.0f,0.0f,1.0f,true),
    m_iConstructor(2)
{
    m_pkPatch = WM4_NEW BSplineRectanglef(iNumUCtrlPoints,iNumVCtrlPoints,
        aakCtrlPoint,iUDegree,iVDegree,bULoop,bVLoop,bUOpen,afVKnot);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3f** aakCtrlPoint, int iUDegree, int iVDegree,
    bool bULoop, bool bVLoop, float* afUKnot, bool bVOpen)
    :
    SurfacePatch(0.0f,1.0f,0.0f,1.0f,true),
    m_iConstructor(3)
{
    m_pkPatch = WM4_NEW BSplineRectanglef(iNumUCtrlPoints,iNumVCtrlPoints,
        aakCtrlPoint,iUDegree,iVDegree,bULoop,bVLoop,afUKnot,bVOpen);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::BSplineSurfacePatch (int iNumUCtrlPoints,
    int iNumVCtrlPoints, Vector3f** aakCtrlPoint, int iUDegree, int iVDegree,
    bool bULoop, bool bVLoop, float* afUKnot, float* afVKnot)
    :
    SurfacePatch(0.0f,1.0f,0.0f,1.0f,true),
    m_iConstructor(4)
{
    m_pkPatch = WM4_NEW BSplineRectanglef(iNumUCtrlPoints,iNumVCtrlPoints,
        aakCtrlPoint,iUDegree,iVDegree,bULoop,bVLoop,afUKnot,afVKnot);
}
//----------------------------------------------------------------------------
BSplineSurfacePatch::~BSplineSurfacePatch ()
{
    WM4_DELETE m_pkPatch;
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::P (float fU, float fV) const
{
    return m_pkPatch->P(fU,fV);
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::PU (float fU, float fV) const
{
    return m_pkPatch->PU(fU,fV);
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::PV (float fU, float fV) const
{
    return m_pkPatch->PV(fU,fV);
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::PUU (float fU, float fV) const
{
    return m_pkPatch->PUU(fU,fV);
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::PUV (float fU, float fV) const
{
    return m_pkPatch->PUV(fU,fV);
}
//----------------------------------------------------------------------------
Vector3f BSplineSurfacePatch::PVV (float fU, float fV) const
{
    return m_pkPatch->PVV(fU,fV);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    SurfacePatch::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iConstructor);

    int iNumCtrlPoints0, iNumCtrlPoints1;
    rkStream.Read(iNumCtrlPoints0);
    rkStream.Read(iNumCtrlPoints1);
    Vector3f** aakCtrl;
    Allocate<Vector3f>(iNumCtrlPoints1,iNumCtrlPoints0,aakCtrl);
    int i0, i1;
    for (i0 = 0; i0 < iNumCtrlPoints0; i0++)
    {
        for (i1 = 0; i1 < iNumCtrlPoints1; i1++)
        {
            rkStream.Read(aakCtrl[i0][i1]);
        }
    }

    int iDegree0, iDegree1;
    rkStream.Read(iDegree0);
    rkStream.Read(iDegree1);

    bool bLoop0, bLoop1;
    rkStream.Read(bLoop0);
    rkStream.Read(bLoop1);

    bool bOpen0, bOpen1;
    float* afKnot0;
    float* afKnot1;

    switch (m_iConstructor)
    {
    case 1:
        rkStream.Read(bOpen0);
        rkStream.Read(bOpen1);

        m_pkPatch = WM4_NEW BSplineRectanglef(iNumCtrlPoints0,iNumCtrlPoints1,
            aakCtrl,iDegree0,iDegree1,bLoop0,bLoop1,bOpen0,bOpen1);
        break;
    case 2:
        rkStream.Read(bOpen0);

        afKnot1 = WM4_NEW float[iNumCtrlPoints1 - iDegree1 - 1];
        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            rkStream.Read(afKnot1[i1]);
        }

        m_pkPatch = WM4_NEW BSplineRectanglef(iNumCtrlPoints0,iNumCtrlPoints1,
            aakCtrl,iDegree0,iDegree1,bLoop0,bLoop1,bOpen0,afKnot1);

        WM4_DELETE[] afKnot1;
        break;
    case 3:
        afKnot0 = WM4_NEW float[iNumCtrlPoints0 - iDegree0 - 1];
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            rkStream.Read(afKnot0[i0]);
        }

        rkStream.Read(bOpen1);

        m_pkPatch = WM4_NEW BSplineRectanglef(iNumCtrlPoints0,iNumCtrlPoints1,
            aakCtrl,iDegree0,iDegree1,bLoop0,bLoop1,afKnot0,bOpen1);

        WM4_DELETE[] afKnot0;
        break;
    case 4:
        afKnot0 = WM4_NEW float[iNumCtrlPoints0 - iDegree0 - 1];
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            rkStream.Read(afKnot0[i0]);
        }

        afKnot1 = WM4_NEW float[iNumCtrlPoints1 - iDegree1 - 1];
        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            rkStream.Read(afKnot1[i1]);
        }

        m_pkPatch = WM4_NEW BSplineRectanglef(iNumCtrlPoints0,iNumCtrlPoints1,
            aakCtrl,iDegree0,iDegree1,bLoop0,bLoop1,afKnot0,afKnot1);

        WM4_DELETE[] afKnot0;
        WM4_DELETE[] afKnot1;
        break;
    default:
        assert(false);
    }

    WM4_END_DEBUG_STREAM_LOAD(BSplineSurfacePatch);
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Link (Stream& rkStream, Stream::Link* pkLink)
{
    SurfacePatch::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BSplineSurfacePatch::Register (Stream& rkStream) const
{
    return SurfacePatch::Register(rkStream);
}
//----------------------------------------------------------------------------
void BSplineSurfacePatch::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    SurfacePatch::Save(rkStream);

    // native data
    rkStream.Write(m_iConstructor);

    int iNumCtrlPoints0 = m_pkPatch->GetNumCtrlPoints(0);
    int iNumCtrlPoints1 = m_pkPatch->GetNumCtrlPoints(1);
    rkStream.Write(iNumCtrlPoints0);
    rkStream.Write(iNumCtrlPoints1);
    int i0, i1;
    for (i0 = 0; i0 < iNumCtrlPoints0; i0++)
    {
        for (i1 = 0; i1 < iNumCtrlPoints1; i1++)
        {
            rkStream.Write(m_pkPatch->GetControlPoint(i0,i1));
        }
    }

    int iDegree0 = m_pkPatch->GetDegree(0);
    int iDegree1 = m_pkPatch->GetDegree(1);
    rkStream.Write(iDegree0);
    rkStream.Write(iDegree1);

    rkStream.Write(m_pkPatch->IsLoop(0));
    rkStream.Write(m_pkPatch->IsLoop(1));

    switch (m_iConstructor)
    {
    case 1:
        rkStream.Write(m_pkPatch->IsOpen(0));
        rkStream.Write(m_pkPatch->IsOpen(1));
        break;
    case 2:
        rkStream.Write(m_pkPatch->IsOpen(0));

        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            rkStream.Write(m_pkPatch->GetKnot(1,i1));
        }
        break;
    case 3:
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            rkStream.Write(m_pkPatch->GetKnot(0,i0));
        }

        rkStream.Write(m_pkPatch->IsOpen(1));
        break;
    case 4:
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            rkStream.Write(m_pkPatch->GetKnot(0,i0));
        }

        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            rkStream.Write(m_pkPatch->GetKnot(1,i1));
        }
        break;
    default:
        assert(false);
    }

    WM4_END_DEBUG_STREAM_SAVE(SurfacePatch);
}
//----------------------------------------------------------------------------
int BSplineSurfacePatch::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = SurfacePatch::GetDiskUsed(rkVersion) +
        sizeof(m_iConstructor) +
        4*sizeof(int) +  // numctrl0, numctrl1, deg0, deg1
        2*sizeof(char);  // loop0, loop1

    switch (m_iConstructor)
    {
    case 1:
        iSize += 2*sizeof(char);  // open0, open1
        break;
    case 2:
        iSize += sizeof(char);  // open0
        iSize += sizeof(float)*(m_pkPatch->GetNumCtrlPoints(1) -
            m_pkPatch->GetDegree(1) - 1);  // knot1[]
        break;
    case 3:
        iSize += sizeof(float)*(m_pkPatch->GetNumCtrlPoints(0) -
            m_pkPatch->GetDegree(0) - 1);  // knot0[]
        iSize += sizeof(char);  // open1
        break;
    case 4:
        iSize += sizeof(float)*(m_pkPatch->GetNumCtrlPoints(0) -
            m_pkPatch->GetDegree(0) - 1);  // knot0[]
        iSize += sizeof(float)*(m_pkPatch->GetNumCtrlPoints(1) -
            m_pkPatch->GetDegree(1) - 1);  // knot1[]
        break;
    default:
        assert(false);
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* BSplineSurfacePatch::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));

    int iNumCtrlPoints0 = m_pkPatch->GetNumCtrlPoints(0);
    int iNumCtrlPoints1 = m_pkPatch->GetNumCtrlPoints(1);
    pkTree->Append(Format("ctrl0 quantity =",iNumCtrlPoints0));
    pkTree->Append(Format("ctrl1 quantity =",iNumCtrlPoints1));
    int iDegree0 = m_pkPatch->GetDegree(0);
    int iDegree1 = m_pkPatch->GetDegree(1);
    pkTree->Append(Format("degree0 =",iDegree0));
    pkTree->Append(Format("degree1 =",iDegree1));
    pkTree->Append(Format("loop0 =",m_pkPatch->IsLoop(0)));
    pkTree->Append(Format("loop1 =",m_pkPatch->IsLoop(1)));

    const size_t uiTitleSize = 16;
    char acTitle[uiTitleSize];
    int i0, i1;
    for (i0 = 0; i0 < iNumCtrlPoints0; i0++)
    {
        for (i1 = 0; i1 < iNumCtrlPoints1; i1++)
        {
            System::Sprintf(acTitle,uiTitleSize,"ctrl(%d,%d) =",i0,i1);
            pkTree->Append(Format(acTitle,m_pkPatch->GetControlPoint(i0,i1)));
        }
    }

    switch (m_iConstructor)
    {
    case 1:
        pkTree->Append(Format("open0 =",m_pkPatch->IsOpen(0)));
        pkTree->Append(Format("open1 =",m_pkPatch->IsOpen(1)));
        break;
    case 2:
        pkTree->Append(Format("open0 =",m_pkPatch->IsOpen(0)));
        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            System::Sprintf(acTitle,uiTitleSize,"knot1(%d) =",i1);
            pkTree->Append(Format(acTitle,m_pkPatch->GetKnot(1,i1)));
        }
        break;
    case 3:
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            System::Sprintf(acTitle,uiTitleSize,"knot0(%d) =",i0);
            pkTree->Append(Format(acTitle,m_pkPatch->GetKnot(0,i0)));
        }
        pkTree->Append(Format("open1 =",m_pkPatch->IsOpen(1)));
        break;
    case 4:
        for (i0 = 0; i0 < iNumCtrlPoints0 - iDegree0 - 1; i0++)
        {
            System::Sprintf(acTitle,uiTitleSize,"knot0(%d) =",i0);
            pkTree->Append(Format(acTitle,m_pkPatch->GetKnot(0,i0)));
        }
        for (i1 = 0; i1 < iNumCtrlPoints1 - iDegree1 - 1; i1++)
        {
            System::Sprintf(acTitle,uiTitleSize,"knot1(%d) =",i1);
            pkTree->Append(Format(acTitle,m_pkPatch->GetKnot(1,i1)));
        }
        break;
    default:
        assert(false);
    }

    // children
    pkTree->Append(SurfacePatch::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
