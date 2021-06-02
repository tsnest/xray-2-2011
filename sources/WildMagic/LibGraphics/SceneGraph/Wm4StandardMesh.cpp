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
// Version: 4.0.3 (2009/01/24)

#include "Wm4GraphicsPCH.h"
#include "Wm4StandardMesh.h"
#include "Wm4BSplineSurfacePatch.h"
#include "Wm4EdgeKey.h"
#include "Wm4SurfaceMesh.h"
using namespace Wm4;

//----------------------------------------------------------------------------
StandardMesh::StandardMesh (const Attributes& rkAttr, bool bInside,
    const Transformation* pkXFrm)
    :
    m_kAttr(rkAttr),
    m_bInside(bInside)
{
    assert(m_kAttr.GetPositionChannels() == 3);
    if (m_kAttr.HasNormal())
    {
        assert(m_kAttr.GetNormalChannels() == 3);
    }

    for (int iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
    {
        if (m_kAttr.HasTCoord(iUnit))
        {
            assert(m_kAttr.GetTCoordChannels(iUnit) == 2);
        }
    }

    if (pkXFrm)
    {
        m_kXFrm = *pkXFrm;
    }

    m_pkVBuffer = 0;
    m_pkIBuffer = 0;
}
//----------------------------------------------------------------------------
StandardMesh::~StandardMesh ()
{
}
//----------------------------------------------------------------------------
void StandardMesh::SetTransformation (const Transformation& rkXFrm)
{
    m_kXFrm = rkXFrm;
}
//----------------------------------------------------------------------------
const Transformation& StandardMesh::GetTransformation () const
{
    return m_kXFrm;
}
//----------------------------------------------------------------------------
void StandardMesh::TransformData (VertexBuffer* pkVB)
{
    if (m_kXFrm.IsIdentity())
    {
        return;
    }

    int iVQuantity = pkVB->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Position3(i) = m_kXFrm.ApplyForward(pkVB->Position3(i));
    }

    if (m_kAttr.HasNormal())
    {
        Vector3f kSave = m_kXFrm.GetTranslate();
        m_kXFrm.SetTranslate(Vector3f::ZERO);
        for (i = 0; i < iVQuantity; i++)
        {
            pkVB->Normal3(i) = m_kXFrm.ApplyForward(pkVB->Normal3(i));
            pkVB->Normal3(i).Normalize();
        }
        m_kXFrm.SetTranslate(kSave);
    }
}
//----------------------------------------------------------------------------
void StandardMesh::ReverseTriangleOrder (int iTQuantity, int* aiIndex)
{
    for (int i = 0; i < iTQuantity; i++)
    {
        int j1 = 3*i+1, j2 = j1+1;
        int iSave = aiIndex[j1];
        aiIndex[j1] = aiIndex[j2];
        aiIndex[j2] = iSave;
    }
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Rectangle (int iXSamples, int iYSamples,
    float fXExtent, float fYExtent)
{
    int iVQuantity = iXSamples*iYSamples;
    int iTQuantity = 2*(iXSamples-1)*(iYSamples-1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    float fInv0 = 1.0f/(iXSamples - 1.0f);
    float fInv1 = 1.0f/(iYSamples - 1.0f);
    float fU, fV;
    int i, i0, i1;
    for (i1 = 0, i = 0; i1 < iYSamples; i1++)
    {
        fV = i1*fInv1;
        Vector3f kYTmp = ((2.0f*fV-1.0f)*fYExtent)*Vector3f::UNIT_Y;
        for (i0 = 0; i0 < iXSamples; i0++)
        {
            fU = i0*fInv0;
            Vector3f kXTmp = ((2.0f*fU-1.0f)*fXExtent)*Vector3f::UNIT_X;
            pkVB->Position3(i) = kXTmp + kYTmp;

            if (m_kAttr.HasNormal())
            {
                pkVB->Normal3(i) = Vector3f::UNIT_Z;
            }

            if (m_kAttr.GetMaxTCoords() > 0)
            {
                Vector2f kTCoord(fU,fV);
                for (int iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                {
                    if (m_kAttr.HasTCoord(iUnit))
                    {
                        pkVB->TCoord2(iUnit,i) = kTCoord;
                    }
                }
            }

            i++;
        }
    }

    // generate connectivity
    int* aiIndex = pkIB->GetData();
    for (i1 = 0, i = 0; i1 < iYSamples - 1; i1++)
    {
        for (i0 = 0; i0 < iXSamples - 1; i0++)
        {
            int iV0 = i0 + iXSamples * i1;
            int iV1 = iV0 + 1;
            int iV2 = iV1 + iXSamples;
            int iV3 = iV0 + iXSamples;
            aiIndex[i++] = iV0;
            aiIndex[i++] = iV1;
            aiIndex[i++] = iV2;
            aiIndex[i++] = iV0;
            aiIndex[i++] = iV2;
            aiIndex[i++] = iV3;
        }
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Disk (int iShellSamples, int iRadialSamples,
    float fRadius)
{
    int iRSm1 = iRadialSamples - 1, iSSm1 = iShellSamples - 1;
    int iVQuantity = 1 + iRadialSamples*iSSm1;
    int iTQuantity = iRadialSamples*(2*iSSm1-1);
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    int iR, iS, i, iUnit;
    Vector2f kTCoord;

    // center of disk
    pkVB->Position3(0) = Vector3f::ZERO;
    if (m_kAttr.HasNormal())
    {
        pkVB->Normal3(0) = Vector3f::UNIT_Z;
    }

    if (m_kAttr.GetMaxTCoords() > 0)
    {
        kTCoord = Vector2f(0.5f,0.5f);
        for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
        {
            if (m_kAttr.HasTCoord(iUnit))
            {
                pkVB->TCoord2(iUnit,0) = kTCoord;
            }
        }
    }

    float fInvSSm1 = 1.0f/(float)iSSm1;
    float fInvRS = 1.0f/(float)iRadialSamples;
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        float fCos = Mathf::Cos(fAngle);
        float fSin = Mathf::Sin(fAngle);
        Vector3f kRadial(fCos,fSin,0.0f);

        for (iS = 1; iS < iShellSamples; iS++)
        {
            float fFraction = fInvSSm1*iS;  // in (0,R]
            Vector3f kFracRadial = fFraction*kRadial;
            i = iS+iSSm1*iR;
            pkVB->Position3(i) = fRadius*kFracRadial;
            if (m_kAttr.HasNormal())
            {
                pkVB->Normal3(i) = Vector3f::UNIT_Z;
            }

            if (m_kAttr.GetMaxTCoords() > 0)
            {
                kTCoord = 0.5f*Vector2f(1.0f+kFracRadial.X(),
                    1.0f+kFracRadial.Y());
                for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                {
                    if (m_kAttr.HasTCoord(iUnit))
                    {
                        pkVB->TCoord2(iUnit,i) = kTCoord;
                    }
                }
            }
        }
    }

    // generate connectivity
    int* aiLocalIndex = pkIB->GetData();
    int iT = 0;
    for (int iR0 = iRSm1, iR1 = 0; iR1 < iRadialSamples; iR0 = iR1++)
    {
        aiLocalIndex[0] = 0;
        aiLocalIndex[1] = 1+iSSm1*iR0;
        aiLocalIndex[2] = 1+iSSm1*iR1;
        aiLocalIndex += 3;
        iT++;
        for (int iS = 1; iS < iSSm1; iS++, aiLocalIndex += 6)
        {
            int i00 = iS+iSSm1*iR0;
            int i01 = iS+iSSm1*iR1;
            int i10 = i00+1;
            int i11 = i01+1;
            aiLocalIndex[0] = i00;
            aiLocalIndex[1] = i10;
            aiLocalIndex[2] = i11;
            aiLocalIndex[3] = i00;
            aiLocalIndex[4] = i11;
            aiLocalIndex[5] = i01;
            iT += 2;
        }
    }
    assert(iT == iTQuantity);

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Box (float fXExtent, float fYExtent, float fZExtent)
{
    int iVQuantity = 8;
    int iTQuantity = 12;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    pkVB->Position3(0) = Vector3f(-fXExtent,-fYExtent,-fZExtent);
    pkVB->Position3(1) = Vector3f(+fXExtent,-fYExtent,-fZExtent);
    pkVB->Position3(2) = Vector3f(+fXExtent,+fYExtent,-fZExtent);
    pkVB->Position3(3) = Vector3f(-fXExtent,+fYExtent,-fZExtent);
    pkVB->Position3(4) = Vector3f(-fXExtent,-fYExtent,+fZExtent);
    pkVB->Position3(5) = Vector3f(+fXExtent,-fYExtent,+fZExtent);
    pkVB->Position3(6) = Vector3f(+fXExtent,+fYExtent,+fZExtent);
    pkVB->Position3(7) = Vector3f(-fXExtent,+fYExtent,+fZExtent);

    if (m_kAttr.GetMaxTCoords() > 0)
    {
        for (int iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
        {
            if (m_kAttr.HasTCoord(iUnit))
            {
                pkVB->TCoord2(iUnit,0) = Vector2f(0.25f,0.75f);
                pkVB->TCoord2(iUnit,1) = Vector2f(0.75f,0.75f);
                pkVB->TCoord2(iUnit,2) = Vector2f(0.75f,0.25f);
                pkVB->TCoord2(iUnit,3) = Vector2f(0.25f,0.25f);
                pkVB->TCoord2(iUnit,4) = Vector2f(0.0f,1.0f);
                pkVB->TCoord2(iUnit,5) = Vector2f(1.0f,1.0f);
                pkVB->TCoord2(iUnit,6) = Vector2f(1.0f,0.0f);
                pkVB->TCoord2(iUnit,7) = Vector2f(0.0f,0.0f);
            }
        }
    }

    // generate connectivity (outside view)
    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 2;  aiIndex[ 2] = 1;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 3;  aiIndex[ 5] = 2;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 1;  aiIndex[ 8] = 5;
    aiIndex[ 9] = 0;  aiIndex[10] = 5;  aiIndex[11] = 4;
    aiIndex[12] = 0;  aiIndex[13] = 4;  aiIndex[14] = 7;
    aiIndex[15] = 0;  aiIndex[16] = 7;  aiIndex[17] = 3;
    aiIndex[18] = 6;  aiIndex[19] = 4;  aiIndex[20] = 5;
    aiIndex[21] = 6;  aiIndex[22] = 7;  aiIndex[23] = 4;
    aiIndex[24] = 6;  aiIndex[25] = 5;  aiIndex[26] = 1;
    aiIndex[27] = 6;  aiIndex[28] = 1;  aiIndex[29] = 2;
    aiIndex[30] = 6;  aiIndex[31] = 2;  aiIndex[32] = 3;
    aiIndex[33] = 6;  aiIndex[34] = 3;  aiIndex[35] = 7;

    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    pkMesh->UpdateMS(true);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Cylinder (int iAxisSamples, int iRadialSamples,
    float fRadius, float fHeight, bool bOpen)
{
    TriMesh* pkMesh;

    if (bOpen)
    {
        int iVQuantity = iAxisSamples*(iRadialSamples+1);
        int iTQuantity = 2*(iAxisSamples-1)*iRadialSamples;
        VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
        IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

        // generate geometry
        float fInvRS = 1.0f/(float)iRadialSamples;
        float fInvASm1 = 1.0f/(float)(iAxisSamples-1);
        float fHalfHeight = 0.5f*fHeight;
        int iR, iA, iAStart, i, iUnit;
        Vector2f kTCoord;

        // Generate points on the unit circle to be used in computing the
        // mesh points on a cylinder slice.
        float* afSin = WM4_NEW float[iRadialSamples+1];
        float* afCos = WM4_NEW float[iRadialSamples+1];
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fAngle = Mathf::TWO_PI*fInvRS*iR;
            afCos[iR] = Mathf::Cos(fAngle);
            afSin[iR] = Mathf::Sin(fAngle);
        }
        afSin[iRadialSamples] = afSin[0];
        afCos[iRadialSamples] = afCos[0];

        // generate the cylinder itself
        for (iA = 0, i = 0; iA < iAxisSamples; iA++)
        {
            float fAxisFraction = iA*fInvASm1;  // in [0,1]
            float fZ = -fHalfHeight + fHeight*fAxisFraction;

            // compute center of slice
            Vector3f kSliceCenter(0.0f,0.0f,fZ);

            // compute slice vertices with duplication at end point
            int iSave = i;
            for (iR = 0; iR < iRadialSamples; iR++)
            {
                float fRadialFraction = iR*fInvRS;  // in [0,1)
                Vector3f kNormal(afCos[iR],afSin[iR],0.0f);
                pkVB->Position3(i) = kSliceCenter + fRadius*kNormal;
                if (m_kAttr.HasNormal())
                {
                    if (m_bInside)
                    {
                        pkVB->Normal3(i) = -kNormal;
                    }
                    else
                    {
                        pkVB->Normal3(i) = kNormal;
                    }
                }

                if (m_kAttr.GetMaxTCoords() > 0)
                {
                    kTCoord = Vector2f(fRadialFraction,fAxisFraction);
                    for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                    {
                        if (m_kAttr.HasTCoord(iUnit))
                        {
                            pkVB->TCoord2(iUnit,i) = kTCoord;
                        }
                    }
                }

                i++;
            }

            pkVB->Position3(i) = pkVB->Position3(iSave);
            if (m_kAttr.HasNormal())
            {
                pkVB->Normal3(i) = pkVB->Normal3(iSave);
            }

            if (m_kAttr.GetMaxTCoords() > 0)
            {
                kTCoord = Vector2f(1.0f,fAxisFraction);
                for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                {
                    if (m_kAttr.HasTCoord(iUnit))
                    {
                        pkVB->TCoord2(iUnit,i) = kTCoord;
                    }
                }
            }

            i++;
        }

        // generate connectivity
        int* aiLocalIndex = pkIB->GetData();
        for (iA = 0, iAStart = 0; iA < iAxisSamples-1; iA++)
        {
            int i0 = iAStart;
            int i1 = i0 + 1;
            iAStart += iRadialSamples + 1;
            int i2 = iAStart;
            int i3 = i2 + 1;
            for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
            {
                if (m_bInside)
                {
                    aiLocalIndex[0] = i0++;
                    aiLocalIndex[1] = i2;
                    aiLocalIndex[2] = i1;
                    aiLocalIndex[3] = i1++;
                    aiLocalIndex[4] = i2++;
                    aiLocalIndex[5] = i3++;
                }
                else // outside view
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

        WM4_DELETE[] afCos;
        WM4_DELETE[] afSin;

        TransformData(pkVB);
        pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    }
    else
    {
        pkMesh = Sphere(iAxisSamples,iRadialSamples,fRadius);
        VertexBuffer* pkVB = pkMesh->VBuffer;
        int iVQuantity = pkVB->GetVertexQuantity();

        // flatten sphere at poles
        float fHDiv2 = 0.5f*fHeight;
        pkVB->Position3(iVQuantity-2).Z() = -fHDiv2;  // south pole
        pkVB->Position3(iVQuantity-1).Z() = fHDiv2;  // north pole

        // remap z-values to [-h/2,h/2]
        float fZFactor = 2.0f/(iAxisSamples-1);
        float fTmp0 = fRadius*(-1.0f + fZFactor);
        float fTmp1 = 1.0f/(fRadius*(+1.0f - fZFactor));
        for (int i = 0; i < iVQuantity-2; i++)
        {
            pkVB->Position3(i).Z() = fHDiv2*(-1.0f+fTmp1*(
                pkVB->Position3(i).Z()-fTmp0));
            float fX = pkVB->Position3(i).X(), fY = pkVB->Position3(i).Y();
            float fAdjust = fRadius*Mathf::InvSqrt(fX*fX + fY*fY);
            pkVB->Position3(i).X() *= fAdjust;
            pkVB->Position3(i).Y() *= fAdjust;
        }
        TransformData(pkVB);
        pkMesh->UpdateMS(true);
    }

    // The duplication of vertices at the seam cause the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    float fMaxDist = Mathf::Sqrt(fRadius*fRadius+fHeight*fHeight);
    pkMesh->ModelBound->SetCenter(Vector3f::ZERO);
    pkMesh->ModelBound->SetRadius(fMaxDist);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Sphere (int iZSamples, int iRadialSamples,
    float fRadius)
{
    int iZSm1 = iZSamples-1, iZSm2 = iZSamples-2, iZSm3 = iZSamples-3;
    int iRSp1 = iRadialSamples+1;
    int iVQuantity = iZSm2*iRSp1 + 2;
    int iTQuantity = 2*iZSm2*iRadialSamples;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    float fInvRS = 1.0f/(float)iRadialSamples;
    float fZFactor = 2.0f/(float)iZSm1;
    int iR, iZ, iZStart, i, iUnit;
    Vector2f kTCoord;

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    float* afSin = WM4_NEW float[iRSp1];
    float* afCos = WM4_NEW float[iRSp1];
    for (iR = 0; iR < iRadialSamples; iR++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*iR;
        afCos[iR] = Mathf::Cos(fAngle);
        afSin[iR] = Mathf::Sin(fAngle);
    }
    afSin[iRadialSamples] = afSin[0];
    afCos[iRadialSamples] = afCos[0];

    // generate the cylinder itself
    for (iZ = 1, i = 0; iZ < iZSm1; iZ++)
    {
        float fZFraction = -1.0f + fZFactor*iZ;  // in (-1,1)
        float fZ = fRadius*fZFraction;

        // compute center of slice
        Vector3f kSliceCenter(0.0f,0.0f,fZ);

        // compute radius of slice
        float fSliceRadius = Mathf::Sqrt(Mathf::FAbs(fRadius*fRadius-fZ*fZ));

        // compute slice vertices with duplication at end point
        Vector3f kNormal;
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fRadialFraction = iR*fInvRS;  // in [0,1)
            Vector3f kRadial(afCos[iR],afSin[iR],0.0f);
            pkVB->Position3(i) = kSliceCenter + fSliceRadius*kRadial;
            if (m_kAttr.HasNormal())
            {
                kNormal = pkVB->Position3(i);
                kNormal.Normalize();
                if (m_bInside)
                {
                    pkVB->Normal3(i) = -kNormal;
                }
                else
                {
                    pkVB->Normal3(i) = kNormal;
                }
            }

            if (m_kAttr.GetMaxTCoords() > 0)
            {
                kTCoord = Vector2f(fRadialFraction,
                    0.5f*(fZFraction+1.0f));
                for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                {
                    if (m_kAttr.HasTCoord(iUnit))
                    {
                        pkVB->TCoord2(iUnit,i) = kTCoord;
                    }
                }
            }

            i++;
        }

        pkVB->Position3(i) = pkVB->Position3(iSave);
        if (m_kAttr.HasNormal())
        {
            pkVB->Normal3(i) = pkVB->Normal3(iSave);
        }

        if (m_kAttr.GetMaxTCoords() > 0)
        {
            kTCoord = Vector2f(1.0f,0.5f*(fZFraction+1.0f));
            for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
            {
                if (m_kAttr.HasTCoord(iUnit))
                {
                    pkVB->TCoord2(iUnit,i) = kTCoord;
                }
            }
        }

        i++;
    }

    // south pole
    pkVB->Position3(i) = -fRadius*Vector3f::UNIT_Z;
    if (m_kAttr.HasNormal())
    {
        if (m_bInside)
        {
            pkVB->Normal3(i) = Vector3f::UNIT_Z;
        }
        else
        {
            pkVB->Normal3(i) = -Vector3f::UNIT_Z;
        }
    }

    if (m_kAttr.GetMaxTCoords() > 0)
    {
        kTCoord = Vector2f(0.5f,0.5f);
        for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
        {
            if (m_kAttr.HasTCoord(iUnit))
            {
                pkVB->TCoord2(iUnit,i) = kTCoord;
            }
        }
    }

    i++;

    // north pole
    pkVB->Position3(i) = fRadius*Vector3f::UNIT_Z;
    if (m_kAttr.HasNormal())
    {
        if (m_bInside)
        {
            pkVB->Normal3(i) = -Vector3f::UNIT_Z;
        }
        else
        {
            pkVB->Normal3(i) = Vector3f::UNIT_Z;
        }
    }

    if (m_kAttr.GetMaxTCoords() > 0)
    {
        kTCoord = Vector2f(0.5f,1.0f);
        for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
        {
            if (m_kAttr.HasTCoord(iUnit))
            {
                pkVB->TCoord2(iUnit,i) = kTCoord;
            }
        }
    }

    i++;
    assert(i == iVQuantity);

    // generate connectivity
    int* aiLocalIndex = pkIB->GetData();
    for (iZ = 0, iZStart = 0; iZ < iZSm3; iZ++)
    {
        int i0 = iZStart;
        int i1 = i0 + 1;
        iZStart += iRSp1;
        int i2 = iZStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            if (m_bInside)
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
            else  // inside view
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

    // south pole triangles
    int iVQm2 = iVQuantity-2;
    for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 3)
    {
        if (m_bInside)
        {
            aiLocalIndex[0] = i;
            aiLocalIndex[1] = i+1;
            aiLocalIndex[2] = iVQm2;
        }
        else  // inside view
        {
            aiLocalIndex[0] = i;
            aiLocalIndex[1] = iVQm2;
            aiLocalIndex[2] = i+1;
        }
    }

    // north pole triangles
    int iVQm1 = iVQuantity-1, iOffset = iZSm3*iRSp1;
    for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 3)
    {
        if (m_bInside)
        {
            aiLocalIndex[0] = i+iOffset;
            aiLocalIndex[1] = iVQm1;
            aiLocalIndex[2] = i+1+iOffset;
        }
        else  // inside view
        {
            aiLocalIndex[0] = i+iOffset;
            aiLocalIndex[1] = i+1+iOffset;
            aiLocalIndex[2] = iVQm1;
        }
    }

    assert(aiLocalIndex == pkIB->GetData() + 3*iTQuantity);

    WM4_DELETE[] afCos;
    WM4_DELETE[] afSin;

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);

    // The duplication of vertices at the seam cause the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    pkMesh->ModelBound->SetCenter(Vector3f::ZERO);
    pkMesh->ModelBound->SetRadius(fRadius);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Torus (int iCircleSamples, int iRadialSamples,
    float fOuterRadius, float fInnerRadius)
{
    int iVQuantity = (iCircleSamples+1)*(iRadialSamples+1);
    int iTQuantity = 2*iCircleSamples*iRadialSamples;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // generate geometry
    float fInvCS = 1.0f/(float)iCircleSamples;
    float fInvRS = 1.0f/(float)iRadialSamples;
    int iC, iR, i, iUnit;
    Vector2f kTCoord;

    // generate the cylinder itself
    for (iC = 0, i = 0; iC < iCircleSamples; iC++)
    {
        // compute center point on torus circle at specified angle
        float fCircleFraction = iC*fInvCS;  // in [0,1)
        float fTheta = Mathf::TWO_PI*fCircleFraction;
        float fCosTheta = Mathf::Cos(fTheta);
        float fSinTheta = Mathf::Sin(fTheta);
        Vector3f kRadial(fCosTheta,fSinTheta,0.0f);
        Vector3f kTorusMiddle = fOuterRadius*kRadial;

        // compute slice vertices with duplication at end point
        int iSave = i;
        for (iR = 0; iR < iRadialSamples; iR++)
        {
            float fRadialFraction = iR*fInvRS;  // in [0,1)
            float fPhi = Mathf::TWO_PI*fRadialFraction;
            float fCosPhi = Mathf::Cos(fPhi);
            float fSinPhi = Mathf::Sin(fPhi);
            Vector3f kNormal = fCosPhi*kRadial + fSinPhi*Vector3f::UNIT_Z;
            pkVB->Position3(i) = kTorusMiddle + fInnerRadius*kNormal;
            if (m_kAttr.HasNormal())
            {
                if (m_bInside)
                {
                    pkVB->Normal3(i) = -kNormal;
                }
                else
                {
                    pkVB->Normal3(i) = kNormal;
                }
            }

            if (m_kAttr.GetMaxTCoords() > 0)
            {
                kTCoord = Vector2f(fRadialFraction,fCircleFraction);
                for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
                {
                    if (m_kAttr.HasTCoord(iUnit))
                    {
                        pkVB->TCoord2(iUnit,i) = kTCoord;
                    }
                }
            }

            i++;
        }

        pkVB->Position3(i) = pkVB->Position3(iSave);
        if (m_kAttr.HasNormal())
        {
            pkVB->Normal3(i) = pkVB->Normal3(iSave);
        }

        if (m_kAttr.GetMaxTCoords() > 0)
        {
            kTCoord = Vector2f(1.0f,fCircleFraction);
            for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
            {
                if (m_kAttr.HasTCoord(iUnit))
                {
                    pkVB->TCoord2(iUnit,i) = kTCoord;
                }
            }
        }

        i++;
    }

    // duplicate the cylinder ends to form a torus
    for (iR = 0; iR <= iRadialSamples; iR++, i++)
    {
        pkVB->Position3(i) = pkVB->Position3(iR);
        if (m_kAttr.HasNormal())
        {
            pkVB->Normal3(i) = pkVB->Normal3(iR);
        }

        if (m_kAttr.GetMaxTCoords() > 0)
        {
            kTCoord = Vector2f(pkVB->TCoord2(0,iR).X(),1.0f);
            for (iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
            {
                if (m_kAttr.HasTCoord(iUnit))
                {
                    pkVB->TCoord2(iUnit,i) = kTCoord;
                }
            }
        }
    }
    assert(i == iVQuantity);

    // generate connectivity
    int* aiLocalIndex = pkIB->GetData();
    int iCStart = 0;
    for (iC = 0; iC < iCircleSamples; iC++)
    {
        int i0 = iCStart;
        int i1 = i0 + 1;
        iCStart += iRadialSamples + 1;
        int i2 = iCStart;
        int i3 = i2 + 1;
        for (i = 0; i < iRadialSamples; i++, aiLocalIndex += 6)
        {
            if (m_bInside)
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
            else  // inside view
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

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);

    // The duplication of vertices at the seam cause the automatically
    // generated bounding volume to be slightly off center.  Reset the bound
    // to use the true information.
    pkMesh->ModelBound->SetCenter(Vector3f::ZERO);
    pkMesh->ModelBound->SetRadius(fOuterRadius);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Tetrahedron ()
{
    float fSqrt2Div3 = Mathf::Sqrt(2.0f)/3.0f;
    float fSqrt6Div3 = Mathf::Sqrt(6.0f)/3.0f;
    float fOneThird = 1.0f/3.0f;

    int iVQuantity = 4;
    int iTQuantity = 4;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    pkVB->Position3(0) = Vector3f(0.0f,0.0f,1.0f);
    pkVB->Position3(1) = Vector3f(2.0f*fSqrt2Div3,0.0f,-fOneThird);
    pkVB->Position3(2) = Vector3f(-fSqrt2Div3,fSqrt6Div3,-fOneThird);
    pkVB->Position3(3) = Vector3f(-fSqrt2Div3,-fSqrt6Div3,-fOneThird);

    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 1;  aiIndex[ 2] = 2;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 2;  aiIndex[ 5] = 3;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 3;  aiIndex[ 8] = 1;
    aiIndex[ 9] = 1;  aiIndex[10] = 3;  aiIndex[11] = 2;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Hexahedron ()
{
    float fSqrtThird = Mathf::Sqrt(1.0f/3.0f);

    int iVQuantity = 8;
    int iTQuantity = 12;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    pkVB->Position3(0) = Vector3f(-fSqrtThird,-fSqrtThird,-fSqrtThird);
    pkVB->Position3(1) = Vector3f( fSqrtThird,-fSqrtThird,-fSqrtThird);
    pkVB->Position3(2) = Vector3f( fSqrtThird, fSqrtThird,-fSqrtThird);
    pkVB->Position3(3) = Vector3f(-fSqrtThird, fSqrtThird,-fSqrtThird);
    pkVB->Position3(4) = Vector3f(-fSqrtThird,-fSqrtThird, fSqrtThird);
    pkVB->Position3(5) = Vector3f( fSqrtThird,-fSqrtThird, fSqrtThird);
    pkVB->Position3(6) = Vector3f( fSqrtThird, fSqrtThird, fSqrtThird);
    pkVB->Position3(7) = Vector3f(-fSqrtThird, fSqrtThird, fSqrtThird);

    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] = 0;  aiIndex[ 1] = 3;  aiIndex[ 2] = 2;
    aiIndex[ 3] = 0;  aiIndex[ 4] = 2;  aiIndex[ 5] = 1;
    aiIndex[ 6] = 0;  aiIndex[ 7] = 1;  aiIndex[ 8] = 5;
    aiIndex[ 9] = 0;  aiIndex[10] = 5;  aiIndex[11] = 4;
    aiIndex[12] = 0;  aiIndex[13] = 4;  aiIndex[14] = 7;
    aiIndex[15] = 0;  aiIndex[16] = 7;  aiIndex[17] = 3;
    aiIndex[18] = 6;  aiIndex[19] = 5;  aiIndex[20] = 1;
    aiIndex[21] = 6;  aiIndex[22] = 1;  aiIndex[23] = 2;
    aiIndex[24] = 6;  aiIndex[25] = 2;  aiIndex[26] = 3;
    aiIndex[27] = 6;  aiIndex[28] = 3;  aiIndex[29] = 7;
    aiIndex[30] = 6;  aiIndex[31] = 7;  aiIndex[32] = 4;
    aiIndex[33] = 6;  aiIndex[34] = 4;  aiIndex[35] = 5;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Octahedron ()
{
    int iVQuantity = 6;
    int iTQuantity = 8;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    pkVB->Position3(0) = Vector3f( 1.0f, 0.0f, 0.0f);
    pkVB->Position3(1) = Vector3f(-1.0f, 0.0f, 0.0f);
    pkVB->Position3(2) = Vector3f( 0.0f, 1.0f, 0.0f);
    pkVB->Position3(3) = Vector3f( 0.0f,-1.0f, 0.0f);
    pkVB->Position3(4) = Vector3f( 0.0f, 0.0f, 1.0f);
    pkVB->Position3(5) = Vector3f( 0.0f, 0.0f,-1.0f);

    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] = 4;  aiIndex[ 1] = 0;  aiIndex[ 2] = 2;
    aiIndex[ 3] = 4;  aiIndex[ 4] = 2;  aiIndex[ 5] = 1;
    aiIndex[ 6] = 4;  aiIndex[ 7] = 1;  aiIndex[ 8] = 3;
    aiIndex[ 9] = 4;  aiIndex[10] = 3;  aiIndex[11] = 0;
    aiIndex[12] = 5;  aiIndex[13] = 2;  aiIndex[14] = 0;
    aiIndex[15] = 5;  aiIndex[16] = 1;  aiIndex[17] = 2;
    aiIndex[18] = 5;  aiIndex[19] = 3;  aiIndex[20] = 1;
    aiIndex[21] = 5;  aiIndex[22] = 0;  aiIndex[23] = 3;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Dodecahedron ()
{
    float fA = 1.0f/Mathf::Sqrt(3.0f);
    float fB = Mathf::Sqrt((3.0f-Mathf::Sqrt(5.0f))/6.0f);
    float fC = Mathf::Sqrt((3.0f+Mathf::Sqrt(5.0f))/6.0f);

    int iVQuantity = 20;
    int iTQuantity = 36;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    pkVB->Position3( 0) = Vector3f( fA, fA, fA);
    pkVB->Position3( 1) = Vector3f( fA, fA,-fA);
    pkVB->Position3( 2) = Vector3f( fA,-fA, fA);
    pkVB->Position3( 3) = Vector3f( fA,-fA,-fA);
    pkVB->Position3( 4) = Vector3f(-fA, fA, fA);
    pkVB->Position3( 5) = Vector3f(-fA, fA,-fA);
    pkVB->Position3( 6) = Vector3f(-fA,-fA, fA);
    pkVB->Position3( 7) = Vector3f(-fA,-fA,-fA);
    pkVB->Position3( 8) = Vector3f(  fB,  fC, 0.0f);
    pkVB->Position3( 9) = Vector3f( -fB,  fC, 0.0f);
    pkVB->Position3(10) = Vector3f(  fB, -fC, 0.0f);
    pkVB->Position3(11) = Vector3f( -fB, -fC, 0.0f);
    pkVB->Position3(12) = Vector3f(  fC, 0.0f,  fB);
    pkVB->Position3(13) = Vector3f(  fC, 0.0f, -fB);
    pkVB->Position3(14) = Vector3f( -fC, 0.0f,  fB);
    pkVB->Position3(15) = Vector3f( -fC, 0.0f, -fB);
    pkVB->Position3(16) = Vector3f(0.0f,   fB,  fC);
    pkVB->Position3(17) = Vector3f(0.0f,  -fB,  fC);
    pkVB->Position3(18) = Vector3f(0.0f,   fB, -fC);
    pkVB->Position3(19) = Vector3f(0.0f,  -fB, -fC);

    int* aiIndex = pkIB->GetData();
    aiIndex[  0] =  0;  aiIndex[  1] =  8;  aiIndex[  2] =  9;
    aiIndex[  3] =  0;  aiIndex[  4] =  9;  aiIndex[  5] =  4;
    aiIndex[  6] =  0;  aiIndex[  7] =  4;  aiIndex[  8] = 16;
    aiIndex[  9] =  0;  aiIndex[ 10] = 12;  aiIndex[ 11] = 13;
    aiIndex[ 12] =  0;  aiIndex[ 13] = 13;  aiIndex[ 14] =  1;
    aiIndex[ 15] =  0;  aiIndex[ 16] =  1;  aiIndex[ 17] =  8;
    aiIndex[ 18] =  0;  aiIndex[ 19] = 16;  aiIndex[ 20] = 17;
    aiIndex[ 21] =  0;  aiIndex[ 22] = 17;  aiIndex[ 23] =  2;
    aiIndex[ 24] =  0;  aiIndex[ 25] =  2;  aiIndex[ 26] = 12;
    aiIndex[ 27] =  8;  aiIndex[ 28] =  1;  aiIndex[ 29] = 18;
    aiIndex[ 30] =  8;  aiIndex[ 31] = 18;  aiIndex[ 32] =  5;
    aiIndex[ 33] =  8;  aiIndex[ 34] =  5;  aiIndex[ 35] =  9;
    aiIndex[ 36] = 12;  aiIndex[ 37] =  2;  aiIndex[ 38] = 10;
    aiIndex[ 39] = 12;  aiIndex[ 40] = 10;  aiIndex[ 41] =  3;
    aiIndex[ 42] = 12;  aiIndex[ 43] =  3;  aiIndex[ 44] = 13;
    aiIndex[ 45] = 16;  aiIndex[ 46] =  4;  aiIndex[ 47] = 14;
    aiIndex[ 48] = 16;  aiIndex[ 49] = 14;  aiIndex[ 50] =  6;
    aiIndex[ 51] = 16;  aiIndex[ 52] =  6;  aiIndex[ 53] = 17;
    aiIndex[ 54] =  9;  aiIndex[ 55] =  5;  aiIndex[ 56] = 15;
    aiIndex[ 57] =  9;  aiIndex[ 58] = 15;  aiIndex[ 59] = 14;
    aiIndex[ 60] =  9;  aiIndex[ 61] = 14;  aiIndex[ 62] =  4;
    aiIndex[ 63] =  6;  aiIndex[ 64] = 11;  aiIndex[ 65] = 10;
    aiIndex[ 66] =  6;  aiIndex[ 67] = 10;  aiIndex[ 68] =  2;
    aiIndex[ 69] =  6;  aiIndex[ 70] =  2;  aiIndex[ 71] = 17;
    aiIndex[ 72] =  3;  aiIndex[ 73] = 19;  aiIndex[ 74] = 18;
    aiIndex[ 75] =  3;  aiIndex[ 76] = 18;  aiIndex[ 77] =  1;
    aiIndex[ 78] =  3;  aiIndex[ 79] =  1;  aiIndex[ 80] = 13;
    aiIndex[ 81] =  7;  aiIndex[ 82] = 15;  aiIndex[ 83] =  5;
    aiIndex[ 84] =  7;  aiIndex[ 85] =  5;  aiIndex[ 86] = 18;
    aiIndex[ 87] =  7;  aiIndex[ 88] = 18;  aiIndex[ 89] = 19;
    aiIndex[ 90] =  7;  aiIndex[ 91] = 11;  aiIndex[ 92] =  6;
    aiIndex[ 93] =  7;  aiIndex[ 94] =  6;  aiIndex[ 95] = 14;
    aiIndex[ 96] =  7;  aiIndex[ 97] = 14;  aiIndex[ 98] = 15;
    aiIndex[ 99] =  7;  aiIndex[100] = 19;  aiIndex[101] =  3;
    aiIndex[102] =  7;  aiIndex[103] =  3;  aiIndex[104] = 10;
    aiIndex[105] =  7;  aiIndex[106] = 10;  aiIndex[107] = 11;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Icosahedron ()
{
    float fGoldenRatio = 0.5f*(1.0f+Mathf::Sqrt(5.0f));
    float fInvRoot = 1.0f/Mathf::Sqrt(1.0f+fGoldenRatio*fGoldenRatio);
    float fU = fGoldenRatio*fInvRoot;
    float fV = fInvRoot;

    int iVQuantity = 12;
    int iTQuantity = 20;
    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    pkVB->Position3( 0) = Vector3f(  fU,  fV,0.0f);
    pkVB->Position3( 1) = Vector3f( -fU,  fV,0.0f);
    pkVB->Position3( 2) = Vector3f(  fU, -fV,0.0f);
    pkVB->Position3( 3) = Vector3f( -fU, -fV,0.0f);
    pkVB->Position3( 4) = Vector3f(  fV,0.0f,  fU);
    pkVB->Position3( 5) = Vector3f(  fV,0.0f, -fU);
    pkVB->Position3( 6) = Vector3f( -fV,0.0f,  fU);
    pkVB->Position3( 7) = Vector3f( -fV,0.0f, -fU);
    pkVB->Position3( 8) = Vector3f(0.0f,  fU,  fV);
    pkVB->Position3( 9) = Vector3f(0.0f, -fU,  fV);
    pkVB->Position3(10) = Vector3f(0.0f,  fU, -fV);
    pkVB->Position3(11) = Vector3f(0.0f, -fU, -fV);

    int* aiIndex = pkIB->GetData();
    aiIndex[ 0] =  0;  aiIndex[ 1] =  8;  aiIndex[ 2] =  4;
    aiIndex[ 3] =  0;  aiIndex[ 4] =  5;  aiIndex[ 5] = 10;
    aiIndex[ 6] =  2;  aiIndex[ 7] =  4;  aiIndex[ 8] =  9;
    aiIndex[ 9] =  2;  aiIndex[10] = 11;  aiIndex[11] =  5;
    aiIndex[12] =  1;  aiIndex[13] =  6;  aiIndex[14] =  8;
    aiIndex[15] =  1;  aiIndex[16] = 10;  aiIndex[17] =  7;
    aiIndex[18] =  3;  aiIndex[19] =  9;  aiIndex[20] =  6;
    aiIndex[21] =  3;  aiIndex[22] =  7;  aiIndex[23] = 11;
    aiIndex[24] =  0;  aiIndex[25] = 10;  aiIndex[26] =  8;
    aiIndex[27] =  1;  aiIndex[28] =  8;  aiIndex[29] = 10;
    aiIndex[30] =  2;  aiIndex[31] =  9;  aiIndex[32] = 11;
    aiIndex[33] =  3;  aiIndex[34] = 11;  aiIndex[35] =  9;
    aiIndex[36] =  4;  aiIndex[37] =  2;  aiIndex[38] =  0;
    aiIndex[39] =  5;  aiIndex[40] =  0;  aiIndex[41] =  2;
    aiIndex[42] =  6;  aiIndex[43] =  1;  aiIndex[44] =  3;
    aiIndex[45] =  7;  aiIndex[46] =  3;  aiIndex[47] =  1;
    aiIndex[48] =  8;  aiIndex[49] =  6;  aiIndex[50] =  4;
    aiIndex[51] =  9;  aiIndex[52] =  4;  aiIndex[53] =  6;
    aiIndex[54] = 10;  aiIndex[55] =  5;  aiIndex[56] =  7;
    aiIndex[57] = 11;  aiIndex[58] =  7;  aiIndex[59] =  5;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
TriMesh* StandardMesh::Geosphere (int iSubdivisions)
{
    assert(iSubdivisions >= 1);

    float fGoldenRatio = 0.5f*(1.0f + Mathf::Sqrt(5.0f));
    float fInvRoot = Mathf::InvSqrt(1.0f + fGoldenRatio*fGoldenRatio);
    float fU = fGoldenRatio*fInvRoot;
    float fV = fInvRoot;

    // Calculate the final number of vertices in the Geosphere.
    int n = iSubdivisions - 2;
    int iVQuantity = 12 + 30*(iSubdivisions-1) + 20*(n*(n+1)/2);

    // Each original icosahedron edge will contain this many vertices after
    // subdivision.
    int iEdgeVQuantity = iSubdivisions + 1;

    // Calculate the final number of faces in the geosphere.
    int iTQuantity = 20*iSubdivisions*iSubdivisions;

    // The face connectivity for an icosahedron.
    int* aiIndex = WM4_NEW int[60];
    aiIndex[ 0] =  0;  aiIndex[ 1] =  8;  aiIndex[ 2] =  4;
    aiIndex[ 3] =  0;  aiIndex[ 4] =  5;  aiIndex[ 5] = 10;
    aiIndex[ 6] =  2;  aiIndex[ 7] =  4;  aiIndex[ 8] =  9;
    aiIndex[ 9] =  2;  aiIndex[10] = 11;  aiIndex[11] =  5;
    aiIndex[12] =  1;  aiIndex[13] =  6;  aiIndex[14] =  8;
    aiIndex[15] =  1;  aiIndex[16] = 10;  aiIndex[17] =  7;
    aiIndex[18] =  3;  aiIndex[19] =  9;  aiIndex[20] =  6;
    aiIndex[21] =  3;  aiIndex[22] =  7;  aiIndex[23] = 11;
    aiIndex[24] =  0;  aiIndex[25] = 10;  aiIndex[26] =  8;
    aiIndex[27] =  1;  aiIndex[28] =  8;  aiIndex[29] = 10;
    aiIndex[30] =  2;  aiIndex[31] =  9;  aiIndex[32] = 11;
    aiIndex[33] =  3;  aiIndex[34] = 11;  aiIndex[35] =  9;
    aiIndex[36] =  4;  aiIndex[37] =  2;  aiIndex[38] =  0;
    aiIndex[39] =  5;  aiIndex[40] =  0;  aiIndex[41] =  2;
    aiIndex[42] =  6;  aiIndex[43] =  1;  aiIndex[44] =  3;
    aiIndex[45] =  7;  aiIndex[46] =  3;  aiIndex[47] =  1;
    aiIndex[48] =  8;  aiIndex[49] =  6;  aiIndex[50] =  4;
    aiIndex[51] =  9;  aiIndex[52] =  4;  aiIndex[53] =  6;
    aiIndex[54] = 10;  aiIndex[55] =  5;  aiIndex[56] =  7;
    aiIndex[57] = 11;  aiIndex[58] =  7;  aiIndex[59] =  5;

    VertexBuffer* pkVB = WM4_NEW VertexBuffer(m_kAttr,iVQuantity);
    IndexBuffer* pkIB = WM4_NEW IndexBuffer(3*iTQuantity);

    // Counters for keeping track of the current vertex, edge or triangle
    // that is being processed or generated.
    int iVCurrent = 0;
    int iECurrent = 0;
    int iTCurrent = 0;

    // The first 12 vertices are the original icosahedron vertices.  The next
    // 30*(iSubdivisions-1) vertices are shared edge vertices.  The 20 sets of
    // vertices after that will contain the all the internal vertices per
    // icosahedron face.

    pkVB->Position3(iVCurrent++) = Vector3f(  fU,  fV,0.0f);
    pkVB->Position3(iVCurrent++) = Vector3f( -fU,  fV,0.0f);
    pkVB->Position3(iVCurrent++) = Vector3f(  fU, -fV,0.0f);
    pkVB->Position3(iVCurrent++) = Vector3f( -fU, -fV,0.0f);
    pkVB->Position3(iVCurrent++) = Vector3f(  fV,0.0f,  fU);
    pkVB->Position3(iVCurrent++) = Vector3f(  fV,0.0f, -fU);
    pkVB->Position3(iVCurrent++) = Vector3f( -fV,0.0f,  fU);
    pkVB->Position3(iVCurrent++) = Vector3f( -fV,0.0f, -fU);
    pkVB->Position3(iVCurrent++) = Vector3f(0.0f,  fU,  fV);
    pkVB->Position3(iVCurrent++) = Vector3f(0.0f, -fU,  fV);
    pkVB->Position3(iVCurrent++) = Vector3f(0.0f,  fU, -fV);
    pkVB->Position3(iVCurrent++) = Vector3f(0.0f, -fU, -fV);

    // Used to retrieve the shared edge vertices later
    std::map<EdgeKey,int> kEdgeMap;

    // Iterate over each icosahedron face.
    int i;
    for (i = 0; i < 20; i++)
    {
        // The current icosahedron face.
        int* aiFace = aiIndex + 3*i;

        // Subdivide each Icosahedron edge, and store the new vertices
        // The map makes sure they are shared between faces
        int j, k;
        for (j = 0; j < 3; j++)
        {
            EdgeKey kEdge(aiFace[j],aiFace[(j+1)%3]);

            // Try to insert the edge. Store the starting index as well.
            std::pair<std::map<EdgeKey,int>::iterator,bool> kRE;
            kRE = kEdgeMap.insert(std::make_pair(kEdge,iVCurrent));

            // If the insertion was made, calculate the additional vertices.
            // Since EdgeKey sorts the indices, we might need to reverse the
            // order in which we access them later.
            if (kRE.second)
            {
                Vector3f kBegin = pkVB->Position3(kEdge.V[0]);
                Vector3f kEnd = pkVB->Position3(kEdge.V[1]);
                Vector3f kStep = (kEnd - kBegin)/(float)iSubdivisions;
                for (k = 1; k < iSubdivisions; k++)
                {
                    pkVB->Position3(iVCurrent++) = kBegin + kStep*(float)k;
                }

                iECurrent++;
            }
        }

        // This will store all the indices for the subdivided triangles.  This
        // is like the diagonal of a matrix and everything below it.
        int** aaiIndices = WM4_NEW int*[iEdgeVQuantity];
        for (j = 0; j < iEdgeVQuantity; j++)
        { 
            aaiIndices[j] = WM4_NEW int[iEdgeVQuantity-j];
        }

        // Fill out the corner indices.
        aaiIndices[0][0] = aiFace[0];
        aaiIndices[0][iSubdivisions] = aiFace[1];
        aaiIndices[iSubdivisions][0] = aiFace[2];

        // Fill out edge01 internal indices.
        int iEdgeIndices = kEdgeMap[EdgeKey(aiFace[0],aiFace[1])];
        for (j = 1; j < iSubdivisions; j++)
        {
            bool bReverse = (aiFace[0] > aiFace[1]);
            if (bReverse)
            {
                aaiIndices[0][j] = iEdgeIndices + iSubdivisions - j - 1;
            }
            else
            {
                aaiIndices[0][j] = iEdgeIndices + j - 1;
            }
        }

        // Fill out edge12 internal indices.
        iEdgeIndices = kEdgeMap[EdgeKey(aiFace[1],aiFace[2])];
        for (j = 1; j < iSubdivisions; j++)
        {
            bool bReverse = (aiFace[1] > aiFace[2]);
            if (bReverse)
            {
                aaiIndices[j][iSubdivisions-j] = 
                    iEdgeIndices + iSubdivisions - j - 1;
            }
            else
            {
                aaiIndices[j][iSubdivisions-j] = iEdgeIndices + j - 1;
            }
        }

        // Fill out edge20 internal indices.
        iEdgeIndices = kEdgeMap[EdgeKey(aiFace[2],aiFace[0])];
        for (j = 1; j < iSubdivisions; j++)
        {
            bool bReverse = (aiFace[2] > aiFace[0]);
            if (bReverse)
            {
                aaiIndices[iSubdivisions-j][0] = 
                    iEdgeIndices + iSubdivisions - j - 1;
            }
            else
            {
                aaiIndices[iSubdivisions-j][0] = iEdgeIndices + j - 1;
            }
        }

        // Now we need to create the internal vertices.
        for (j = 2; j < iSubdivisions; j++)
        {
            Vector3f kBegin = pkVB->Position3(aaiIndices[j][0]);
            Vector3f kEnd = pkVB->Position3(aaiIndices[0][j]);
            Vector3f kStep = (kEnd - kBegin)/(float)j;

            for (k = 1; k < j; k++)
            {
                pkVB->Position3(iVCurrent) = kBegin + kStep*(float)k;
                aaiIndices[j-k][k] = iVCurrent;
                iVCurrent++;
            }
        }

        // Now generate the indices.
        for (j = 0; j < iSubdivisions; j++)
        {
            for (k = 0; k < iSubdivisions-j; k++)
            {
                int* aiFace = pkIB->GetData() + 3*iTCurrent;
                aiFace[0] = aaiIndices[j][k];
                aiFace[1] = aaiIndices[j][k+1];
                aiFace[2] = aaiIndices[j+1][k];
                iTCurrent++;

                if (k+1 != iSubdivisions-j)
                {
                    aiFace[3] = aaiIndices[j][k+1];
                    aiFace[4] = aaiIndices[j+1][k+1];
                    aiFace[5] = aaiIndices[j+1][k];
                    iTCurrent++;
                }
            }
        }

        // Clean up.
        for (j = 0; j < iEdgeVQuantity; j++)
        {
            WM4_DELETE[] aaiIndices[j];
        }
        WM4_DELETE[] aaiIndices;
    }

    // Normalize all the generated vertices, so they lie on the unit sphere
    for (i = 12; i < iVQuantity; i++)
    {
        pkVB->Position3(i).Normalize();
    }

    assert(iVCurrent == iVQuantity);
    assert(iECurrent == 30);
    assert(iTCurrent == iTQuantity);
    assert(kEdgeMap.size() == 30);

    WM4_DELETE[] aiIndex;

    CreatePlatonicNormals(pkVB);
    CreatePlatonicUVs(pkVB);
    if (m_bInside)
    {
        ReverseTriangleOrder(iTQuantity,aiIndex);
    }

    TransformData(pkVB);
    TriMesh* pkMesh = WM4_NEW TriMesh(pkVB,pkIB);
    return pkMesh;
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicNormals (VertexBuffer* pkVBuffer)
{
    if (m_kAttr.HasNormal())
    {
        for (int i = 0; i < pkVBuffer->GetVertexQuantity(); i++)
        {
            pkVBuffer->Normal3(i) = pkVBuffer->Position3(i);
        }
    }
}
//----------------------------------------------------------------------------
void StandardMesh::CreatePlatonicUVs (VertexBuffer* pkVBuffer)
{
    if (m_kAttr.GetMaxTCoords() > 0)
    {
        for (int iUnit = 0; iUnit < m_kAttr.GetMaxTCoords(); iUnit++)
        {
            if (m_kAttr.HasTCoord(iUnit))
            {
                for (int i = 0; i < pkVBuffer->GetVertexQuantity(); i++)
                {
                    if (Mathf::FAbs(pkVBuffer->Position3(i).Z()) < 1.0f)
                    {
                        pkVBuffer->TCoord2(iUnit,i).X() = 0.5f*(1.0f +
                            Mathf::ATan2(pkVBuffer->Position3(i).Y(),
                            pkVBuffer->Position3(i).X())*Mathf::INV_PI);
                    }
                    else
                    {
                        pkVBuffer->TCoord2(iUnit,i).X() = 0.5f;
                    }
                    pkVBuffer->TCoord2(iUnit,i).Y() = Mathf::ACos(
                        pkVBuffer->Position3(i).Z())*Mathf::INV_PI;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
