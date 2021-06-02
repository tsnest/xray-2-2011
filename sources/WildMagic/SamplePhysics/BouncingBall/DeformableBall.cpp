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

#include "DeformableBall.h"
#include "Wm4ExtractSurfaceCubes.h"
#include "Wm4TextureEffect.h"
using namespace Wm4;

//----------------------------------------------------------------------------
DeformableBall::DeformableBall (float fDuration, float fPeriod)
{
    Set(fDuration,fPeriod);
    m_bDeforming = false;
    m_bDoAffine = true;
    CreateBall();
}
//----------------------------------------------------------------------------
DeformableBall::~DeformableBall ()
{
    WM4_DELETE[] m_akNormal;
    WM4_DELETE[] m_akMean;
    WM4_DELETE[] m_aiNeighborCount;
}
//----------------------------------------------------------------------------
void DeformableBall::Set (float fDuration, float fPeriod)
{
    m_fDuration = fDuration;
    m_fDeformMult = 4.0f/(m_fDuration*m_fDuration);
    m_fPeriod = fPeriod;
    m_fMinActive = 0.5f*(m_fPeriod - m_fDuration);
    m_fMaxActive = 0.5f*(m_fPeriod + m_fDuration);
    m_fInvActiveRange = 1.0f/(m_fMaxActive - m_fMinActive);
}
//----------------------------------------------------------------------------
void DeformableBall::CreateBall ()
{
    // create initial image for surface extraction (16x16x16)
    const int iBound = 16;
    float fInvBm1 = 1.0f/(iBound-1.0f);
    int* aiData = WM4_NEW int[iBound*iBound*iBound];
    ExtractSurfaceCubes kExtract(iBound,iBound,iBound,aiData);

    // scale function values to [-1024,1024]
    const float fImageScale = 1024.0f;

    // Initialize image and extract level surface F=0.  Data stores samples
    // for (x,y,z) in [-1,1]x[-1,1]x[0,2].
    Vector3f kPos;
    int i = 0;
    for (int iZ = 0; iZ < iBound; iZ++)
    {
        kPos.Z() = -0.1f + 2.2f*fInvBm1*iZ;
        for (int iY = 0; iY < iBound; iY++)
        {
            kPos.Y() = -1.1f + 2.2f*fInvBm1*iY;
            for (int iX = 0; iX < iBound; iX++)
            {
                kPos.X() = -1.1f + 2.2f*fInvBm1*iX;

                float fFunc;
                Vector3f kGrad;
                ComputeFunction(kPos,0.0f,fFunc,kGrad);
                aiData[i++] = (int)(fImageScale*fFunc);
            }
        }
    }

    // extract the level surface
    std::vector<Vector3f> kVA;
    std::vector<TriangleKey> kTA;
    kExtract.ExtractContour(0.0f,kVA,kTA);
    kExtract.MakeUnique(kVA,kTA);
    kExtract.OrientTriangles(kVA,kTA,true);

    WM4_DELETE[] aiData;

    // Convert to TriMesh object.  Keep track of the level value of the
    // vertices.  Since a vertex might not be exactly on the level surface,
    // we will use
    //     e = max{|F(x,y,z)| : (x,y,z) is a vertex}
    // as the error tolerance for Newton's method in the level surface
    // evolution.
    Attributes kAttr;
    kAttr.SetPositionChannels(3);
    kAttr.SetTCoordChannels(0,2);
    int iVQuantity = (int)kVA.size();
    VertexBuffer* pkVBuffer = WM4_NEW VertexBuffer(kAttr,iVQuantity);
    float fMaxLevel = 0.0f;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVBuffer->Position3(i).X() = -1.1f + 2.2f*fInvBm1*kVA[i].X();
        pkVBuffer->Position3(i).Y() = -1.1f + 2.2f*fInvBm1*kVA[i].Y();
        pkVBuffer->Position3(i).Z() = -0.1f + 2.2f*fInvBm1*kVA[i].Z();
        float fLevel = pkVBuffer->Position3(i).SquaredLength() -
            2.0f*pkVBuffer->Position3(i).Z();
        if (Mathf::FAbs(fLevel) > fMaxLevel)
        {
            fMaxLevel = Mathf::FAbs(fLevel);
        }

        float fWidth = 0.5f*(1.0f+Mathf::ATan2(pkVBuffer->Position3(i).Y(),
            pkVBuffer->Position3(i).Z())/Mathf::PI);  // in [0,1)
        if (fWidth < 0.0f)
        {
            fWidth = 0.0f;
        }
        else if (fWidth >= 1.0f)
        {
            fWidth = 0.999999f;
        }
        float fHeight = 0.5f*pkVBuffer->Position3(i).Z(); // in [0,1)
        if (fHeight < 0.0f)
        {
            fHeight = 0.0f;
        }
        else if (fHeight >= 1.0f)
        {
            fHeight = 0.999999f;
        }

        pkVBuffer->TCoord2(0,i).X() = fWidth;
        pkVBuffer->TCoord2(0,i).Y() = fHeight;
    }

    int iTQuantity = (int)kTA.size();
    int iIQuantity = 3*iTQuantity;
    IndexBuffer* pkIBuffer = WM4_NEW IndexBuffer(iIQuantity);
    int* piIndex = pkIBuffer->GetData();
    for (i = 0; i < iTQuantity; i++)
    {
        *piIndex++ = kTA[i].V[0];
        *piIndex++ = kTA[i].V[1];
        *piIndex++ = kTA[i].V[2];
    }

    m_spkMesh = WM4_NEW TriMesh(pkVBuffer,pkIBuffer);

    TextureEffect* pkEffect = WM4_NEW TextureEffect("BallTexture");
    m_spkMesh->AttachEffect(pkEffect);

    m_iMaxIterations = 8;
    m_fErrorTolerance = fMaxLevel;
    CreateSmoother();
    Update(0.0f);
}
//----------------------------------------------------------------------------
void DeformableBall::CreateSmoother ()
{
    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();

    m_akNormal = WM4_NEW Vector3f[iVQuantity];
    m_akMean = WM4_NEW Vector3f[iVQuantity];
    m_aiNeighborCount = WM4_NEW int[iVQuantity];

    // count the number of vertex neighbors
    memset(m_aiNeighborCount,0,iVQuantity*sizeof(int));
    int iTQuantity = m_spkMesh->IBuffer->GetIndexQuantity()/3;
    const int* piIndex = m_spkMesh->IBuffer->GetData();
    for (int i = 0; i < iTQuantity; i++)
    {
        m_aiNeighborCount[*piIndex++] += 2;
        m_aiNeighborCount[*piIndex++] += 2;
        m_aiNeighborCount[*piIndex++] += 2;
    }
}
//----------------------------------------------------------------------------
void DeformableBall::Update (float fTime)
{
    int iVQuantity = m_spkMesh->VBuffer->GetVertexQuantity();
    int iTQuantity = m_spkMesh->IBuffer->GetIndexQuantity()/3;
    const int* piIndex = m_spkMesh->IBuffer->GetData();

    memset(m_akNormal,0,iVQuantity*sizeof(Vector3f));
    memset(m_akMean,0,iVQuantity*sizeof(Vector3f));

    int i;
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        Vector3f& rkV0 = m_spkMesh->VBuffer->Position3(iV0);
        Vector3f& rkV1 = m_spkMesh->VBuffer->Position3(iV1);
        Vector3f& rkV2 = m_spkMesh->VBuffer->Position3(iV2);

        Vector3f kEdge1 = rkV1 - rkV0;
        Vector3f kEdge2 = rkV2 - rkV0;
        Vector3f kNormal = kEdge1.Cross(kEdge2);

        m_akNormal[iV0] += kNormal;
        m_akNormal[iV1] += kNormal;
        m_akNormal[iV2] += kNormal;

        m_akMean[iV0] += rkV1 + rkV2;
        m_akMean[iV1] += rkV2 + rkV0;
        m_akMean[iV2] += rkV0 + rkV1;
    }

    for (i = 0; i < iVQuantity; i++)
    {
        m_akNormal[i].Normalize();
        m_akMean[i] /= (float)m_aiNeighborCount[i];
    }

    for (i = 0; i < iVQuantity; i++)
    {
        if (VertexInfluenced(i,fTime))
        {
            Vector3f kLocalDiff = m_akMean[i] -
                m_spkMesh->VBuffer->Position3(i);
            Vector3f kSurfaceNormal = kLocalDiff.Dot(m_akNormal[i]) *
                m_akNormal[i];
            Vector3f kTangent = kLocalDiff - kSurfaceNormal;

            float fTWeight = GetTangentWeight(i,fTime);
            float fNWeight = GetNormalWeight(i,fTime);
            m_spkMesh->VBuffer->Position3(i) +=
                fTWeight*kTangent + fNWeight*m_akNormal[i];
        }
    }
}
//----------------------------------------------------------------------------
bool DeformableBall::DoSimulationStep (float fRealTime)
{
    float fTime = fmodf(fRealTime,m_fPeriod);

    if (m_fMinActive < fTime && fTime < m_fMaxActive)
    {
        // deform the mesh
        m_bDeforming = true;
        Update(fTime);

        if (m_bDoAffine)
        {
            // Nonuniform scaling as a hack to make it appear that the body is
            // compressing in the z-direction.  The transformations only
            // affect the display.  If the actual body coordinates were needed
            // for other physics, you would have to modify the mesh vertices.
            //
            // The x- and y-scales vary from 1 to 1.1 to 1 during the time
            // interval [(p-d)/2,(p+d)/2].  The z-scale is the inverse of this
            // scale.  (Expand radially, compress in z-direction.)
            const float fMaxExpand = 0.1f;
            float fAmp = 4.0f*fMaxExpand*m_fInvActiveRange;
            float fXYScale = 1.0f+fAmp*(fTime-m_fMinActive)*
                (m_fMaxActive-fTime);
            float fZScale = 1.0f/fXYScale;
            m_spkMesh->Local.SetScale(Vector3f(fXYScale,fXYScale,fZScale));
        }

        // deformation requires update of bounding sphere
        m_spkMesh->UpdateMS(false);

        // update occurred, application should update the scene graph
        return true;
    }

    if (m_bDeforming)
    {
        // Force restoration of body to its initial state on a transition
        // from deforming to nondeforming.
        m_bDeforming = false;
        Update(0.0f);
        if (m_bDoAffine)
        {
            m_spkMesh->Local.SetRotate(Matrix3f::IDENTITY);
        }
        m_spkMesh->UpdateMS(false);
        return true;
    }

    m_bDeforming = false;
    return false;
}
//----------------------------------------------------------------------------
bool DeformableBall::VertexInfluenced (int i, float fTime)
{
    float fRSqr = m_spkMesh->VBuffer->Position3(i).SquaredLength();
    return fRSqr < 1.0f && m_fMinActive < fTime && fTime < m_fMaxActive;
}
//----------------------------------------------------------------------------
float DeformableBall::GetTangentWeight (int, float)
{
    return 0.5f;
}
//----------------------------------------------------------------------------
float DeformableBall::GetNormalWeight (int i, float fTime)
{
    // find root of F along line origin+s*dir using Newton's method
    float fS = 0.0f;
    for (int iIter = 0; iIter < m_iMaxIterations; iIter++)
    {
        // point of evaluation
        Vector3f kPos = m_spkMesh->VBuffer->Position3(i) + fS*m_akNormal[i];

        // get F(pos,time) and Grad(F)(pos,time)
        float fFunc;
        Vector3f kGrad;
        ComputeFunction(kPos,fTime,fFunc,kGrad);
        if (Mathf::FAbs(fFunc) < m_fErrorTolerance)
        {
            return fS;
        }

        // get directional derivative Dot(dir,Grad(F)(pos,time))
        float fDFunc = m_akNormal[i].Dot(kGrad);
        if (Mathf::FAbs(fDFunc) < m_fErrorTolerance)
        {
            // derivative too close to zero, no change
            return 0.0f;
        }

        fS -= fFunc/fDFunc;
    }

    // method failed to converge within iteration budget, no change
    return 0.0f;
}
//----------------------------------------------------------------------------
void DeformableBall::ComputeFunction (const Vector3f& rkPos, float fTime,
    float& rfFunc, Vector3f& rkGrad)
{
    // Level function is L(X,t) = F(X) + D(X,t) where F(X) = 0 defines the
    // initial body.

    // compute F(X) = x^2 + y^2 + z^2 - 2*z and Grad(F)(X)
    float fRSqr = rkPos.SquaredLength();
    float fF = fRSqr - 2.0f*rkPos.Z();
    Vector3f kFGrad = 2.0f*Vector3f(rkPos.X(),rkPos.Y(),rkPos.Z()-1.0f);

    // Compute D(X,t) = A(t)*G(X).  The duration is d and the period is p.
    // The amplitude is
    //   A(t) = 0, t in [0,p/2-d]
    //          [t-(p/2-d)][(p/2+d)-t]/d^2, t in [p/2-d,p/2+d]
    //          0, t in [p/2+d,p]
    // The spatial component is G(X) = 1 - (x^2 + y^2 + z^2)
    float fD;
    Vector3f kDGrad;
    if (fRSqr < 1.0f && m_fMinActive < fTime && fTime < m_fMaxActive)
    {
        float fAmp = GetAmplitude(fTime);
        fD = fAmp*(1.0f - fRSqr);
        kDGrad = -2.0f*fAmp*rkPos;
    }
    else
    {
        fD = 0.0f;
        kDGrad = Vector3f::ZERO;
    }

    rfFunc = fF + fD;
    rkGrad = kFGrad + kDGrad;
}
//----------------------------------------------------------------------------
