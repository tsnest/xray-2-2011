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
// Version: 4.0.1 (2007/06/16)

#include "Wm4GraphicsPCH.h"
#include "Wm4BoxSurface.h"
#include "Wm4CullState.h"
#include "Wm4ZBufferState.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,BoxSurface,Node);
WM4_IMPLEMENT_STREAM(BoxSurface);
WM4_IMPLEMENT_DEFAULT_NAME_ID(BoxSurface,Node);

//----------------------------------------------------------------------------
BoxSurface::BoxSurface (BSplineVolumef* pkVolume, int iUSamples,
    int iVSamples, int iWSamples, const Attributes& rkAttr, Effect* pkEffect)
{
    assert(pkVolume);

    m_pkVolume = pkVolume;
    m_iUSamples = iUSamples;
    m_iVSamples = iVSamples;
    m_iWSamples = iWSamples;

    int aiPerm[3];
    TriMesh* pkMesh;

    // u faces
    aiPerm[0] = 1;  aiPerm[1] = 2;  aiPerm[2] = 0;

    // u = 0
    pkMesh = CreateFace(m_iWSamples,m_iVSamples,rkAttr,pkEffect,false,0.0f,
        aiPerm);
    pkMesh->SetName("u0");
    AttachChild(pkMesh);

    // u = 1
    pkMesh = CreateFace(m_iWSamples,m_iVSamples,rkAttr,pkEffect,true,1.0f,
        aiPerm);
    pkMesh->SetName("u1");
    AttachChild(pkMesh);

    // v faces
    aiPerm[0] = 0;  aiPerm[1] = 2;  aiPerm[2] = 1;

    // v = 0
    pkMesh = CreateFace(m_iWSamples,m_iUSamples,rkAttr,pkEffect,true,0.0f,
        aiPerm);
    pkMesh->SetName("v0");
    AttachChild(pkMesh);

    // v = 1
    pkMesh = CreateFace(m_iWSamples,m_iUSamples,rkAttr,pkEffect,false,1.0f,
        aiPerm);
    pkMesh->SetName("v1");
    AttachChild(pkMesh);

    // w faces
    aiPerm[0] = 0;  aiPerm[1] = 1;  aiPerm[2] = 2;

    // w = 0
    pkMesh = CreateFace(m_iVSamples,m_iUSamples,rkAttr,pkEffect,false,0.0f,
        aiPerm);
    pkMesh->SetName("w0");
    AttachChild(pkMesh);

    // w = 1
    pkMesh = CreateFace(m_iVSamples,m_iUSamples,rkAttr,pkEffect,true,1.0f,
        aiPerm);
    pkMesh->SetName("w1");
    AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
BoxSurface::BoxSurface ()
{
    m_pkVolume = 0;
}
//----------------------------------------------------------------------------
BoxSurface::~BoxSurface ()
{
    WM4_DELETE m_pkVolume;
}
//----------------------------------------------------------------------------
TriMesh* BoxSurface::CreateFace (int iRows, int iCols,
    const Attributes& rkAttr, Effect* pkEffect, bool bCCW, float fFaceValue,
    int aiPerm[3])
{
    int iVQuantity = iRows*iCols;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(rkAttr,iVQuantity);

    float afP[3];
    afP[aiPerm[2]] = fFaceValue;
    float fSign = (bCCW ? 1.0f : -1.0f);
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    int iRow, iCol, i;
    for (iRow = 0, i = 0; iRow < iRows; iRow++)
    {
        afP[aiPerm[1]] = iRow*fRFactor;
        for (iCol = 0; iCol < iCols; iCol++)
        {
            afP[aiPerm[0]] = iCol*fCFactor;
            pkVB->Position3(i) = m_pkVolume->GetPosition(afP);

            if (rkAttr.HasNormal())
            {
                Vector3f kDC = m_pkVolume->GetDerivative(aiPerm[0],afP);
                Vector3f kDR = m_pkVolume->GetDerivative(aiPerm[1],afP);
                pkVB->Normal3(i) = fSign*kDC.UnitCross(kDR);
            }

            for (int iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
            {
                Vector2f kTCoord(afP[aiPerm[0]],afP[aiPerm[1]]);
                if (rkAttr.HasTCoord(iUnit))
                {
                    assert(rkAttr.GetTCoordChannels(iUnit) == 2);
                    pkVB->TCoord2(iUnit,i) = kTCoord;
                }
            }

            i++;
        }
    }

    int iTQuantity = 2*(iRows-1)*(iCols-1);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);
    int* aiIndex = pkIB->GetData();
    int* aiLocalIndex = aiIndex;
    for (iRow = 0, i = 0; iRow < iRows-1; iRow++)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += iCols;
        int i2 = i;
        int i3 = i2 + 1;
        for (iCol = 0; iCol < iCols-1; iCol++, aiLocalIndex += 6)
        {
            if (bCCW)
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
            else
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
        }
    }

    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    pkMesh->AttachEffect(pkEffect);
    return pkMesh;
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateFace (int iRows, int iCols, VertexBuffer* pkVB,
    bool bCCW, float fFaceValue, int aiPerm[3])
{
    float afP[3];
    afP[aiPerm[2]] = fFaceValue;
    float fSign = (bCCW ? 1.0f : -1.0f);
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    for (int iRow = 0, i = 0; iRow < iRows; iRow++)
    {
        afP[aiPerm[1]] = iRow*fRFactor;
        for (int iCol = 0; iCol < iCols; iCol++)
        {
            afP[aiPerm[0]] = iCol*fCFactor;
            pkVB->Position3(i) = m_pkVolume->GetPosition(afP);

            if (pkVB->GetAttributes().HasNormal())
            {
                Vector3f kDC = m_pkVolume->GetDerivative(aiPerm[0],afP);
                Vector3f kDR = m_pkVolume->GetDerivative(aiPerm[1],afP);
                pkVB->Normal3(i) = fSign*kDC.UnitCross(kDR);
            }

            i++;
        }
    }
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateSurface ()
{
    int aiPerm[3];
    TriMesh* pkMesh;
    VertexBuffer* pkVB;

    // u faces
    aiPerm[0] = 1;  aiPerm[1] = 2;  aiPerm[2] = 0;

    // u = 0
    pkMesh = StaticCast<TriMesh>(GetChild(0));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iWSamples,m_iVSamples,pkVB,false,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // u = 1
    pkMesh = StaticCast<TriMesh>(GetChild(1));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iWSamples,m_iVSamples,pkVB,true,1.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // v faces
    aiPerm[0] = 0;  aiPerm[1] = 2;  aiPerm[2] = 1;

    // v = 0
    pkMesh = StaticCast<TriMesh>(GetChild(2));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iWSamples,m_iUSamples,pkVB,true,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // v = 1
    pkMesh = StaticCast<TriMesh>(GetChild(3));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iWSamples,m_iUSamples,pkVB,false,1.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // w faces
    aiPerm[0] = 0;  aiPerm[1] = 1;  aiPerm[2] = 2;

    // w = 0
    pkMesh = StaticCast<TriMesh>(GetChild(4));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iVSamples,m_iUSamples,pkVB,false,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // w = 1
    pkMesh = StaticCast<TriMesh>(GetChild(5));
    pkVB = pkMesh->VBuffer;
    UpdateFace(m_iVSamples,m_iUSamples,pkVB,true,1.0f,aiPerm);
    pkMesh->UpdateMS(false);
}
//----------------------------------------------------------------------------
void BoxSurface::EnableSorting ()
{
    // no culling (all faces drawn)
    CullState* pkCS = WM4_NEW CullState;
    pkCS->Enabled = false;
    AttachGlobalState(pkCS);

    // write the zbuffer, but do not read (since we are manually sorting)
    ZBufferState* pkZS = WM4_NEW ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = true;
    pkZS->Compare = ZBufferState::CF_LEQUAL;
    AttachGlobalState(pkZS);

    UpdateRS();
    m_bDoSort = true;
}
//----------------------------------------------------------------------------
void BoxSurface::DisableSorting ()
{
    DetachGlobalState(GlobalState::CULL);
    DetachGlobalState(GlobalState::ZBUFFER);
    UpdateRS();
    m_bDoSort = false;
}
//----------------------------------------------------------------------------
void BoxSurface::SortFaces (const Vector3f& rkWorldViewDirection)
{
    if (!m_bDoSort)
    {
        // sorting must be enabled in order to sort...
        return;
    }

    // Inverse transform the world view direction into the local space of
    // the box.
    Vector3f kLocalDir = World.InvertVector(rkWorldViewDirection);

    // Store back-facing faces in the front of the array.  Store front-facing
    // faces in the rear of the array.
    SpatialPtr aspkSort[6];
    int iBackFace = 0, iFrontFace = 5;
    int i;
    for (i = 0; i < 6; i++)
    {
        const char* acName = GetChild(i)->GetName().c_str();
        if (acName[0] == 'u')
        {
            if (acName[1] == '0')
            {
                if (kLocalDir.X() > 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
            else  // acName[1] == '1'
            {
                if (kLocalDir.X() < 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
        }
        else if (acName[0] == 'v')
        {
            if (acName[1] == '0')
            {
                if (kLocalDir.Y() > 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
            else  // acName[1] == '1'
            {
                if (kLocalDir.Y() < 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
        }
        else // acName[0] == 'w'
        {
            if (acName[1] == '0')
            {
                if (kLocalDir.Z() > 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
            else  // acName[1] == '1'
            {
                if (kLocalDir.Z() < 0.0f)
                {
                    aspkSort[iFrontFace--] = GetChild(i);
                }
                else
                {
                    aspkSort[iBackFace++] = GetChild(i);
                }
            }
        }
        DetachChildAt(i);
    }
    assert(iBackFace - iFrontFace == 1);

    // reassign the sorted children to the node parent
    for (i = 0; i < 6; i++)
    {
        SetChild(i,aspkSort[i]);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BoxSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iUSamples);
    rkStream.Read(m_iVSamples);
    rkStream.Read(m_iWSamples);
    rkStream.Read(m_bDoSort);

    // TO DO.  See note in BoxSurface::Save.
    m_pkVolume = 0;

    WM4_END_DEBUG_STREAM_LOAD(BoxSurface);
}
//----------------------------------------------------------------------------
void BoxSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BoxSurface::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void BoxSurface::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_iUSamples);
    rkStream.Write(m_iVSamples);
    rkStream.Write(m_iWSamples);
    rkStream.Write(m_bDoSort);

    // TO DO.  The class BSplineVolume is abstract and does not know
    // about the data representation for the derived-class object that is
    // passed to the BoxSurface constructor.  Because of this, any
    // loaded BoxSurface object will require the application to
    // manually set the volume via the Volume() member function.

    WM4_END_DEBUG_STREAM_SAVE(BoxSurface);
}
//----------------------------------------------------------------------------
int BoxSurface::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Node::GetDiskUsed(rkVersion) +
        sizeof(m_iUSamples) +
        sizeof(m_iVSamples) +
        sizeof(m_iWSamples) +
        sizeof(m_bDoSort);
}
//----------------------------------------------------------------------------
StringTree* BoxSurface::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("u samples =",m_iUSamples));
    pkTree->Append(Format("v samples =",m_iVSamples));
    pkTree->Append(Format("w samples =",m_iWSamples));
    pkTree->Append(Format("sorting enabled =",m_bDoSort));

    // children
    pkTree->Append(Node::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
