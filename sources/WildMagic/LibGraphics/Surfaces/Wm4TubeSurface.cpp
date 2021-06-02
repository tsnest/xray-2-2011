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
#include "Wm4TubeSurface.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,TubeSurface,TriMesh);
WM4_IMPLEMENT_STREAM(TubeSurface);
WM4_IMPLEMENT_DEFAULT_NAME_ID(TubeSurface,TriMesh);

//----------------------------------------------------------------------------
TubeSurface::TubeSurface (Curve3f* pkMedial, RadialFunction oRadial,
    bool bClosed, const Vector3f& rkUpVector, int iMedialSamples,
    int iSliceSamples, const Attributes& rkAttr, bool bSampleByArcLength,
    bool bInsideView, const Vector2f* pkUVMin, const Vector2f* pkUVMax)
{
    assert(pkMedial && oRadial);
    assert((pkUVMin && pkUVMax) || (!pkUVMin && !pkUVMax));

    m_pkMedial = pkMedial;
    m_oRadial = oRadial;
    m_kUpVector = rkUpVector;
    m_iMedialSamples = iMedialSamples;
    m_iSliceSamples = iSliceSamples;
    m_bClosed = bClosed;
    m_bSampleByArcLength = bSampleByArcLength;

    // compute the surface vertices
    int iVQuantity;
    if (m_bClosed)
    {
        iVQuantity = (m_iSliceSamples+1)*(m_iMedialSamples+1);
    }
    else
    {
        iVQuantity = (m_iSliceSamples+1)*m_iMedialSamples;
    }

    VBuffer = WM4_NEW VertexBuffer(rkAttr,iVQuantity);
    ComputeSinCos();
    ComputeVertices();

    // compute the surface normals
    if (rkAttr.HasNormal())
    {
        ComputeNormals();
    }

    // compute the surface textures coordinates
    if (pkUVMin && pkUVMax)
    {
        assert(rkAttr.GetMaxTCoords() > 0);
        ComputeUVs(*pkUVMin,*pkUVMax);
    }

    // compute the surface triangle connectivity
    ComputeIndices(bInsideView);

    UpdateModelBound();
}
//----------------------------------------------------------------------------
TubeSurface::TubeSurface ()
{
    m_pkMedial = 0;
    m_oRadial = 0;
    m_afSin = 0;
    m_afCos = 0;
}
//----------------------------------------------------------------------------
TubeSurface::~TubeSurface ()
{
    WM4_DELETE m_pkMedial;
    WM4_DELETE[] m_afSin;
    WM4_DELETE[] m_afCos;
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeSinCos ()
{
    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.

    m_afSin = WM4_NEW float[m_iSliceSamples + 1];
    m_afCos = WM4_NEW float[m_iSliceSamples + 1];

    float fInvSliceSamples = 1.0f/(float)m_iSliceSamples;
    for (int i = 0; i < m_iSliceSamples; i++)
    {
        float fAngle = Mathf::TWO_PI*fInvSliceSamples*i;
        m_afCos[i] = Mathf::Cos(fAngle);
        m_afSin[i] = Mathf::Sin(fAngle);
    }
    m_afSin[m_iSliceSamples] = m_afSin[0];
    m_afCos[m_iSliceSamples] = m_afCos[0];
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeVertices ()
{
    float fTMin = m_pkMedial->GetMinTime();
    float fTRange = m_pkMedial->GetMaxTime() - fTMin;

    // sampling by arc length requires the total length of the curve
    float fTotalLength;
    if (m_bSampleByArcLength)
    {
        fTotalLength = m_pkMedial->GetTotalLength();
    }
    else
    {
        fTotalLength = 0.0f;
    }

    // vertex construction requires a normalized time (uses a division)
    float fDenom;
    if (m_bClosed)
    {
        fDenom = 1.0f/(float)m_iMedialSamples;
    }
    else
    {
        fDenom = 1.0f/(float)(m_iMedialSamples-1);
    }

    for (int iM = 0, iV = 0; iM < m_iMedialSamples; iM++)
    {
        float fT;
        if (m_bSampleByArcLength)
        {
            fT = m_pkMedial->GetTime(iM*fTotalLength*fDenom);
        }
        else
        {
            fT = fTMin + iM*fTRange*fDenom;
        }

        float fRadius = m_oRadial(fT);

        // compute frame (position P, tangent T, normal N, binormal B)
        Vector3f kP, kT, kN, kB;
        if (m_kUpVector != Vector3f::ZERO)
        {
            // Always use 'up' vector N rather than curve normal.  You must
            // constrain the curve so that T and N are never parallel.  To
            // build the frame from this, let
            //     B = Cross(T,N)/Length(Cross(T,N))
            // and replace
            //     N = Cross(B,T)/Length(Cross(B,T)).
            kP = m_pkMedial->GetPosition(fT);
            kT = m_pkMedial->GetTangent(fT);
            kB = kT.UnitCross(m_kUpVector);
            kN = kB.UnitCross(kT);
        }
        else
        {
            // use Frenet frame to create slices
            m_pkMedial->GetFrame(fT,kP,kT,kN,kB);
        }

        // compute slice vertices, duplication at end point as noted earlier
        int iSave = iV;
        for (int i = 0; i < m_iSliceSamples; i++)
        {
            VBuffer->Position3(iV) = kP + fRadius*(m_afCos[i]*kN +
                m_afSin[i]*kB);
            iV++;
        }
        VBuffer->Position3(iV) = VBuffer->Position3(iSave);
        iV++;
    }

    if (m_bClosed)
    {
        for (int i = 0; i <= m_iSliceSamples; i++)
        {
            int i1 = Index(i,m_iMedialSamples);
            int i0 = Index(i,0);
            VBuffer->Position3(i1) = VBuffer->Position3(i0);
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeNormals ()
{
    int iS, iSm, iSp, iM, iMm, iMp;
    Vector3f kDir0, kDir1;

    // interior normals (central differences)
    for (iM = 1; iM <= m_iMedialSamples-2; iM++)
    {
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = (iS > 0 ? iS-1 : m_iSliceSamples-1);
            iSp = iS + 1;
            iMm = iM - 1;
            iMp = iM + 1;
            kDir0 = VBuffer->Position3(Index(iSm,iM)) -
                VBuffer->Position3(Index(iSp,iM));
            kDir1 = VBuffer->Position3(Index(iS,iMm)) -
                VBuffer->Position3(Index(iS,iMp));
            VBuffer->Normal3(Index(iS,iM)) = kDir0.UnitCross(kDir1);
        }
        VBuffer->Normal3(Index(m_iSliceSamples,iM)) =
            VBuffer->Normal3(Index(0,iM));
    }

    // boundary normals
    if (m_bClosed)
    {
        // central differences
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = (iS > 0 ? iS-1 : m_iSliceSamples-1);
            iSp = iS + 1;

            // m = 0
            kDir0 = VBuffer->Position3(Index(iSm,0)) -
                VBuffer->Position3(Index(iSp,0));
            kDir1 = VBuffer->Position3(Index(iS,m_iMedialSamples-1)) -
                VBuffer->Position3(Index(iS,1));
            VBuffer->Normal3(iS) = kDir0.UnitCross(kDir1);

            // m = max
            VBuffer->Normal3(Index(iS,m_iMedialSamples)) =
                VBuffer->Normal3(Index(iS,0));
        }
        VBuffer->Normal3(Index(m_iSliceSamples,0)) =
            VBuffer->Normal3(Index(0,0));
        VBuffer->Normal3(Index(m_iSliceSamples,m_iMedialSamples)) =
            VBuffer->Normal3(Index(0,m_iMedialSamples));
    }
    else
    {
        // one-sided finite differences

        // m = 0
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = (iS > 0 ? iS-1 : m_iSliceSamples-1);
            iSp = iS + 1;
            kDir0 = VBuffer->Position3(Index(iSm,0)) -
                VBuffer->Position3(Index(iSp,0));
            kDir1 = VBuffer->Position3(Index(iS,0)) -
                VBuffer->Position3(Index(iS,1));
            VBuffer->Normal3(Index(iS,0)) = kDir0.UnitCross(kDir1);
        }
        VBuffer->Normal3(Index(m_iSliceSamples,0)) =
            VBuffer->Normal3(Index(0,0));

        // m = max-1
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = (iS > 0 ? iS-1 : m_iSliceSamples-1);
            iSp = iS + 1;
            kDir0 = VBuffer->Position3(Index(iSm,m_iMedialSamples-1)) -
                VBuffer->Position3(Index(iSp,m_iMedialSamples-1));
            kDir1 = VBuffer->Position3(Index(iS,m_iMedialSamples-2)) -
                VBuffer->Position3(Index(iS,m_iMedialSamples-1));
            VBuffer->Normal3(iS) = kDir0.UnitCross(kDir1);
        }
        VBuffer->Normal3(Index(m_iSliceSamples,m_iMedialSamples-1)) =
            VBuffer->Normal3(Index(0,m_iMedialSamples-1));
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeUVs (const Vector2f& rkUVMin,
    const Vector2f& rkUVMax)
{
    const Attributes& rkAttr = VBuffer->GetAttributes();
    Vector2f kUVRange = rkUVMax - rkUVMin;
    int iMMax = (m_bClosed ? m_iMedialSamples : m_iMedialSamples - 1);
    int iV = 0;
    for (int iM = 0; iM <= iMMax; iM++)
    {
        float fMRatio = ((float)iM)/((float)iMMax);
        float fMValue = rkUVMin.Y() + fMRatio*kUVRange.Y();
        for (int iS = 0; iS <= m_iSliceSamples; iS++)
        {
            float fSRatio = ((float)iS)/((float)m_iSliceSamples);
            float fSValue = rkUVMin.X() + fSRatio*kUVRange.X();
            Vector2f kTCoord(fSValue,fMValue);
            for (int iUnit = 0; iUnit < rkAttr.GetMaxTCoords(); iUnit++)
            {
                if (rkAttr.HasTCoord(iUnit))
                {
                    assert(rkAttr.GetTCoordChannels(iUnit) == 2);
                    VBuffer->TCoord2(iUnit,iV) = kTCoord;
                }
            }
            iV++;
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::ComputeIndices (bool bInsideView)
{
    int iTQuantity;
    if (m_bClosed)
    {
        iTQuantity = 2*m_iSliceSamples*m_iMedialSamples;
    }
    else
    {
        iTQuantity = 2*m_iSliceSamples*(m_iMedialSamples-1);
    }

    IBuffer = WM4_NEW IndexBuffer(3*iTQuantity);
    int* aiIndex = IBuffer->GetData();

    int iM, iMStart, i0, i1, i2, i3, i;
    int* aiLocalIndex = aiIndex;
    for (iM = 0, iMStart = 0; iM < m_iMedialSamples-1; iM++)
    {
        i0 = iMStart;
        i1 = i0 + 1;
        iMStart += m_iSliceSamples + 1;
        i2 = iMStart;
        i3 = i2 + 1;
        for (i = 0; i < m_iSliceSamples; i++, aiLocalIndex += 6)
        {
            if (bInsideView)
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
            else  // outside view
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
        }
    }

    if (m_bClosed)
    {
        i0 = iMStart;
        i1 = i0 + 1;
        i2 = 0;
        i3 = i2 + 1;
        for (i = 0; i < m_iSliceSamples; i++, aiLocalIndex += 6)
        {
            if (bInsideView)
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
            else  // outside view
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurface::GetTMinSlice (Vector3f* akSlice)
{
    for (int i = 0; i <= m_iSliceSamples; i++)
    {
        akSlice[i] = VBuffer->Position3(i);
    }
}
//----------------------------------------------------------------------------
void TubeSurface::GetTMaxSlice (Vector3f* akSlice)
{
    int j = VBuffer->GetVertexQuantity() - m_iSliceSamples - 1;
    for (int i = 0; i <= m_iSliceSamples; i++, j++)
    {
        akSlice[i] = VBuffer->Position3(j);
    }
}
//----------------------------------------------------------------------------
void TubeSurface::UpdateSurface ()
{
    ComputeVertices();
    UpdateModelBound();

    if (VBuffer->GetAttributes().HasNormal())
    {
        ComputeNormals();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TubeSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    rkStream.Read(m_iMedialSamples);
    rkStream.Read(m_iSliceSamples);
    rkStream.Read(m_kUpVector);

    m_afSin = WM4_NEW float[m_iSliceSamples+1];
    m_afCos = WM4_NEW float[m_iSliceSamples+1];
    rkStream.Read(m_iSliceSamples+1,m_afSin);
    rkStream.Read(m_iSliceSamples+1,m_afCos);

    rkStream.Read(m_bClosed);
    rkStream.Read(m_bSampleByArcLength);

    // TO DO.  See note in TubeSurface::Save.
    m_pkMedial = 0;
    m_oRadial = 0;

    WM4_END_DEBUG_STREAM_LOAD(TubeSurface);
}
//----------------------------------------------------------------------------
void TubeSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TubeSurface::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void TubeSurface::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iMedialSamples);
    rkStream.Write(m_iSliceSamples);
    rkStream.Write(m_kUpVector);
    rkStream.Write(m_iSliceSamples+1,m_afSin);
    rkStream.Write(m_iSliceSamples+1,m_afCos);
    rkStream.Write(m_bClosed);
    rkStream.Write(m_bSampleByArcLength);

    // TO DO.  The class Curve3 is abstract and does not know about the data
    // representation for the derived-class object that is passed to the
    // TubeSurface constructor.  RadialFunction is a type of function.  Saving
    // the function pointer is useless since on loading, there is no current
    // way to 'link' to the correct function pointer.  Because of this, any
    // loaded TubeSurface object will require the application to manually set
    // the curve and function via the Medial() and Radial() members.
    //
    // Streaming support should probably be added to the curve classes and
    // a mechanism for saving function pointers should probably be created.

    WM4_END_DEBUG_STREAM_SAVE(TubeSurface);
}
//----------------------------------------------------------------------------
int TubeSurface::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return TriMesh::GetDiskUsed(rkVersion) +
        sizeof(m_iMedialSamples) +
        sizeof(m_iSliceSamples) +
        sizeof(m_kUpVector) +
        2*(m_iSliceSamples+1)*sizeof(float) +  // m_afSin, m_afCos
        sizeof(char) + // m_bClosed
        sizeof(char);  // m_bSampleByArcLength
}
//----------------------------------------------------------------------------
StringTree* TubeSurface::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("closed =",m_bClosed));
    pkTree->Append(Format("medial samples =",m_iMedialSamples));
    pkTree->Append(Format("slice samples =",m_iSliceSamples));
    pkTree->Append(Format("sample by arc length =",m_bSampleByArcLength));
    pkTree->Append(Format("up vector =",m_kUpVector));

    // children
    pkTree->Append(TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
