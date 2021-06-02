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
// Version: 4.0.1 (2007/05/06)

#include "Wm4FoundationPCH.h"
#include "Wm4IntpQdrNonuniform2.h"
#include "Wm4ContScribeCircle2.h"
#include "Wm4DistVector3Segment3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntpQdrNonuniform2<Real>::IntpQdrNonuniform2 (const Delaunay2<Real>& rkDT,
    Real* afF, Real* afFx, Real* afFy, bool bOwner)
    :
    m_pkDT(&rkDT)
{
    assert(afF && afFx && afFy);
    m_afF = afF;
    m_afFx = afFx;
    m_afFy = afFy;
    m_bFOwner = bOwner;
    m_bFxFyOwner = bOwner;
    ProcessTriangles();
}
//----------------------------------------------------------------------------
template <class Real>
IntpQdrNonuniform2<Real>::IntpQdrNonuniform2 (const Delaunay2<Real>& rkDT,
    Real* afF, bool bOwner)
    :
    m_pkDT(&rkDT)
{
    assert(afF);
    m_afF = afF;
    m_bFOwner = bOwner;
    m_bFxFyOwner = true;
    EstimateDerivatives();
    ProcessTriangles();
}
//----------------------------------------------------------------------------
template <class Real>
IntpQdrNonuniform2<Real>::~IntpQdrNonuniform2 ()
{
    if (m_bFOwner)
    {
        WM4_DELETE[] m_afF;
    }

    if (m_bFxFyOwner)
    {
        WM4_DELETE[] m_afFx;
        WM4_DELETE[] m_afFy;
    }

    WM4_DELETE[] m_akTData;
}
//----------------------------------------------------------------------------
template <class Real>
void IntpQdrNonuniform2<Real>::EstimateDerivatives ()
{
    int iVQuantity = m_pkDT->GetVertexQuantity();
    const Vector2<Real>* akVertex = m_pkDT->GetVertices();
    int iTQuantity = m_pkDT->GetSimplexQuantity();
    const int* piIndex = m_pkDT->GetIndices();

    m_afFx = WM4_NEW Real[iVQuantity];
    m_afFy = WM4_NEW Real[iVQuantity];
    Real* afFz = WM4_NEW Real[iVQuantity];
    memset(m_afFx,0,iVQuantity*sizeof(Real));
    memset(m_afFy,0,iVQuantity*sizeof(Real));
    memset(afFz,0,iVQuantity*sizeof(Real));

    // accumulate normals at spatial locations (averaging process)
    int i;
    for (i = 0; i < iTQuantity; i++)
    {
        // get three vertices of triangle
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;

        // compute normal vector of triangle (with positive z-component)
        Real fDx1 = akVertex[iV1].X() - akVertex[iV0].X();
        Real fDy1 = akVertex[iV1].Y() - akVertex[iV0].Y();
        Real fDz1 = m_afF[iV1] - m_afF[iV0];
        Real fDx2 = akVertex[iV2].X() - akVertex[iV0].X();
        Real fDy2 = akVertex[iV2].Y() - akVertex[iV0].Y();
        Real fDz2 = m_afF[iV2] - m_afF[iV0];
        Real fNx = fDy1*fDz2 - fDy2*fDz1;
        Real fNy = fDz1*fDx2 - fDz2*fDx1;
        Real fNz = fDx1*fDy2 - fDx2*fDy1;
        if (fNz < (Real)0.0)
        {
            fNx = -fNx;
            fNy = -fNy;
            fNz = -fNz;
        }

        m_afFx[iV0] += fNx;  m_afFy[iV0] += fNy;  afFz[iV0] += fNz;
        m_afFx[iV1] += fNx;  m_afFy[iV1] += fNy;  afFz[iV1] += fNz;
        m_afFx[iV2] += fNx;  m_afFy[iV2] += fNy;  afFz[iV2] += fNz;
    }

    // scale the normals to form (x,y,-1)
    for (i = 0; i < iVQuantity; i++)
    {
        if (Math<Real>::FAbs(afFz[i]) > Math<Real>::ZERO_TOLERANCE)
        {
            Real fInv = -((Real)1.0)/afFz[i];
            m_afFx[i] *= fInv;
            m_afFy[i] *= fInv;
        }
        else
        {
            m_afFx[i] = (Real)0.0;
            m_afFy[i] = (Real)0.0;
        }
    }

    WM4_DELETE[] afFz;
}
//----------------------------------------------------------------------------
template <class Real>
void IntpQdrNonuniform2<Real>::ProcessTriangles ()
{
    // Add degenerate triangles to boundary triangles so that interpolation
    // at the boundary can be treated in the same way as interpolation in
    // the interior.

    // compute centers of inscribed circles for triangles
    const Vector2<Real>* akVertex = m_pkDT->GetVertices();
    int iTQuantity = m_pkDT->GetSimplexQuantity();
    const int* piIndex = m_pkDT->GetIndices();
    m_akTData = WM4_NEW TriangleData[iTQuantity];
    int i;
    for (i = 0; i < iTQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        Circle2<Real> kCircle;
        Inscribe(akVertex[iV0],akVertex[iV1],akVertex[iV2],kCircle);
        m_akTData[i].Center = kCircle.Center;
    }

    // compute cross-edge intersections
    for (i = 0; i < iTQuantity; i++)
    {
        ComputeCrossEdgeIntersections(i);
    }

    // compute Bezier coefficients
    for (i = 0; i < iTQuantity; i++)
    {
        ComputeCoefficients(i);
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntpQdrNonuniform2<Real>::ComputeCrossEdgeIntersections (int i)
{
    // get the vertices of triangle i
    Vector2<Real> akV[3];
    m_pkDT->GetVertexSet(i,akV);

    // get centers of adjacent triangles
    int aiAdjacent[3];
    m_pkDT->GetAdjacentSet(i,aiAdjacent);
    Vector2<Real> akU[3];
    for (int j = 0; j < 3; j++)
    {
        int iA = aiAdjacent[j];
        if (iA >= 0)
        {
            // get center of adjacent triangle's circumscribing circle
            akU[j] = m_akTData[iA].Center;
        }
        else
        {
            // no adjacent triangle, use center of edge
            akU[j] = ((Real)0.5)*(akV[(j+2)%3] + akV[(j+1)%3]);
        }
    }

    Real fM00, fM01, fM10, fM11, fR0, fR1, fInvDet;

    // intersection on edge <V0,V1>
    fM00 = akV[0].Y() - akV[1].Y();
    fM01 = akV[1].X() - akV[0].X();
    fM10 = m_akTData[i].Center.Y() - akU[0].Y();
    fM11 = akU[0].X() - m_akTData[i].Center.X();
    fR0  = fM00*akV[0].X() + fM01*akV[0].Y();
    fR1  = fM10*m_akTData[i].Center.X() + fM11*m_akTData[i].Center.Y();
    fInvDet = ((Real)1.0)/(fM00*fM11 - fM01*fM10);
    m_akTData[i].Intersect[0].X() = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[i].Intersect[0].Y() = (fM00*fR1-fM10*fR0)*fInvDet;

    // intersection on edge <V1,V2>
    fM00 = akV[1].Y() - akV[2].Y();
    fM01 = akV[2].X() - akV[1].X();
    fM10 = m_akTData[i].Center.Y() - akU[1].Y();
    fM11 = akU[1].X() - m_akTData[i].Center.X();
    fR0  = fM00*akV[1].X() + fM01*akV[1].Y();
    fR1  = fM10*m_akTData[i].Center.X() + fM11*m_akTData[i].Center.Y();
    fInvDet = ((Real)1.0)/(fM00*fM11 - fM01*fM10);
    m_akTData[i].Intersect[1].X() = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[i].Intersect[1].Y() = (fM00*fR1-fM10*fR0)*fInvDet;

    // intersection on edge <V0,V2>
    fM00 = akV[0].Y() - akV[2].Y();
    fM01 = akV[2].X() - akV[0].X();
    fM10 = m_akTData[i].Center.Y() - akU[2].Y();
    fM11 = akU[2].X() - m_akTData[i].Center.X();
    fR0  = fM00*akV[0].X() + fM01*akV[0].Y();
    fR1  = fM10*m_akTData[i].Center.X() + fM11*m_akTData[i].Center.Y();
    fInvDet = ((Real)1.0)/(fM00*fM11 - fM01*fM10);
    m_akTData[i].Intersect[2].X() = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[i].Intersect[2].Y() = (fM00*fR1-fM10*fR0)*fInvDet;
}
//----------------------------------------------------------------------------
template <class Real>
void IntpQdrNonuniform2<Real>::ComputeCoefficients (int i)
{
    // get the vertices of triangle i
    Vector2<Real> akV[3];
    m_pkDT->GetVertexSet(i,akV);

    // get the vertex indices of triangle i
    int aiIndex[3];
    m_pkDT->GetIndexSet(i,aiIndex);

    // get the additional information for triangle i
    TriangleData& rkTData = m_akTData[i];

    // get the sample data at main triangle vertices
    Jet afJet[3];
    int j;
    for (j = 0; j < 3; j++)
    {
        int k = aiIndex[j];
        afJet[j].F = m_afF[k];
        afJet[j].Fx = m_afFx[k];
        afJet[j].Fy = m_afFy[k];
    }

    // get centers of adjacent triangles
    int aiAdjacent[3];
    m_pkDT->GetAdjacentSet(i,aiAdjacent);
    Vector2<Real> akU[3];
    for (j = 0; j < 3; j++)
    {
        int iA = aiAdjacent[j];
        if (iA >= 0)
        {
            // get center of adjacent triangle's circumscribing circle
            akU[j] = m_akTData[iA].Center;
        }
        else
        {
            // no adjacent triangle, use center of edge
            akU[j] = ((Real)0.5)*(akV[(j+2)%3] + akV[(j+1)%3]);
        }
    }

    // compute intermediate terms
    Real afCenT[3], afCen0[3], afCen1[3], afCen2[3];
    m_pkDT->GetBarycentricSet(i,rkTData.Center,afCenT);
    m_pkDT->GetBarycentricSet(i,akU[0],afCen0);
    m_pkDT->GetBarycentricSet(i,akU[1],afCen1);
    m_pkDT->GetBarycentricSet(i,akU[2],afCen2);

    Real fAlpha = (afCenT[1]*afCen1[0]-afCenT[0]*afCen1[1]) /
        (afCen1[0]-afCenT[0]);
    Real fBeta = (afCenT[2]*afCen2[1]-afCenT[1]*afCen2[2]) /
        (afCen2[1]-afCenT[1]);
    Real fGamma = (afCenT[0]*afCen0[2]-afCenT[2]*afCen0[0]) /
        (afCen0[2]-afCenT[2]);
    Real fOmAlpha = (Real)1.0 - fAlpha;
    Real fOmBeta  = (Real)1.0 - fBeta;
    Real fOmGamma = (Real)1.0 - fGamma;

    Real fTmp, afA[9], afB[9];

    fTmp = afCenT[0]*akV[0].X()+afCenT[1]*akV[1].X()+afCenT[2]*akV[2].X();
    afA[0] = ((Real)0.5)*(fTmp-akV[0].X());
    afA[1] = ((Real)0.5)*(fTmp-akV[1].X());
    afA[2] = ((Real)0.5)*(fTmp-akV[2].X());
    afA[3] = ((Real)0.5)*fBeta*(akV[2].X()-akV[0].X());
    afA[4] = ((Real)0.5)*fOmGamma*(akV[1].X()-akV[0].X());
    afA[5] = ((Real)0.5)*fGamma*(akV[0].X()-akV[1].X());
    afA[6] = ((Real)0.5)*fOmAlpha*(akV[2].X()-akV[1].X());
    afA[7] = ((Real)0.5)*fAlpha*(akV[1].X()-akV[2].X());
    afA[8] = ((Real)0.5)*fOmBeta*(akV[0].X()-akV[2].X());

    fTmp = afCenT[0]*akV[0].Y()+afCenT[1]*akV[1].Y()+afCenT[2]*akV[2].Y();
    afB[0] = ((Real)0.5)*(fTmp-akV[0].Y());
    afB[1] = ((Real)0.5)*(fTmp-akV[1].Y());
    afB[2] = ((Real)0.5)*(fTmp-akV[2].Y());
    afB[3] = ((Real)0.5)*fBeta*(akV[2].Y()-akV[0].Y());
    afB[4] = ((Real)0.5)*fOmGamma*(akV[1].Y()-akV[0].Y());
    afB[5] = ((Real)0.5)*fGamma*(akV[0].Y()-akV[1].Y());
    afB[6] = ((Real)0.5)*fOmAlpha*(akV[2].Y()-akV[1].Y());
    afB[7] = ((Real)0.5)*fAlpha*(akV[1].Y()-akV[2].Y());
    afB[8] = ((Real)0.5)*fOmBeta*(akV[0].Y()-akV[2].Y());

    // compute Bezier coefficients
    rkTData.Coeff[ 2] = afJet[0].F;
    rkTData.Coeff[ 4] = afJet[1].F;
    rkTData.Coeff[ 6] = afJet[2].F;

    rkTData.Coeff[14] = afJet[0].F + afA[0]*afJet[0].Fx + afB[0]*afJet[0].Fy;
    rkTData.Coeff[ 7] = afJet[0].F + afA[3]*afJet[0].Fx + afB[3]*afJet[0].Fy;
    rkTData.Coeff[ 8] = afJet[0].F + afA[4]*afJet[0].Fx + afB[4]*afJet[0].Fy;
    rkTData.Coeff[16] = afJet[1].F + afA[1]*afJet[1].Fx + afB[1]*afJet[1].Fy;
    rkTData.Coeff[ 9] = afJet[1].F + afA[5]*afJet[1].Fx + afB[5]*afJet[1].Fy;
    rkTData.Coeff[10] = afJet[1].F + afA[6]*afJet[1].Fx + afB[6]*afJet[1].Fy;
    rkTData.Coeff[18] = afJet[2].F + afA[2]*afJet[2].Fx + afB[2]*afJet[2].Fy;
    rkTData.Coeff[11] = afJet[2].F + afA[7]*afJet[2].Fx + afB[7]*afJet[2].Fy;
    rkTData.Coeff[12] = afJet[2].F + afA[8]*afJet[2].Fx + afB[8]*afJet[2].Fy;

    rkTData.Coeff[ 5] = fAlpha*rkTData.Coeff[10] + fOmAlpha*rkTData.Coeff[11];
    rkTData.Coeff[17] = fAlpha*rkTData.Coeff[16] + fOmAlpha*rkTData.Coeff[18];
    rkTData.Coeff[ 1] = fBeta*rkTData.Coeff[12]  + fOmBeta*rkTData.Coeff[ 7];
    rkTData.Coeff[13] = fBeta*rkTData.Coeff[18]  + fOmBeta*rkTData.Coeff[14];
    rkTData.Coeff[ 3] = fGamma*rkTData.Coeff[ 8] + fOmGamma*rkTData.Coeff[ 9];
    rkTData.Coeff[15] = fGamma*rkTData.Coeff[14] + fOmGamma*rkTData.Coeff[16];
    rkTData.Coeff[ 0] = afCenT[0]*rkTData.Coeff[14] +
        afCenT[1]*rkTData.Coeff[16] + afCenT[2]*rkTData.Coeff[18];
}
//----------------------------------------------------------------------------
template <class Real>
bool IntpQdrNonuniform2<Real>::Evaluate (const Vector2<Real>& rkP, Real& rfF,
    Real& rfFx, Real& rfFy)
{
    int i = m_pkDT->GetContainingTriangle(rkP);
    if (i == -1)
    {
        return false;
    }

    // get triangle information
    Vector2<Real> akV[3];
    m_pkDT->GetVertexSet(i,akV);
    int aiIndex[3];
    m_pkDT->GetIndexSet(i,aiIndex);
    TriangleData& rkTData = m_akTData[i];

    // determine which of the six subtriangles contains the target point
    Vector2<Real> kSub0 = rkTData.Center;
    Vector2<Real> kSub1;
    Vector2<Real> kSub2 = rkTData.Intersect[2];
    Real afBary[3];
    int iIndex;
    for (iIndex = 1; iIndex <= 6; iIndex++)
    {
        kSub1 = kSub2;
        if (iIndex % 2)
        {
            kSub2 = akV[iIndex/2];
        }
        else
        {
            kSub2 = rkTData.Intersect[iIndex/2-1];
        }

        rkP.GetBarycentrics(kSub0,kSub1,kSub2,afBary);
        if (afBary[0] >= (Real)0.0 && afBary[1] >= (Real)0.0
        &&  afBary[2] >= (Real)0.0)
        {
            // P is in triangle <Sub0,Sub1,Sub2>
            break;
        }
    }

    // This should not happen theoretically, but it can happen due to
    // numerical round-off errors.  Just in case, select an index and go
    // with it.  Probably better is to keep track of the dot products in
    // InTriangle and find the one closest to zero and use a triangle that
    // contains the edge as the one that contains the input point.
    assert(iIndex <= 6);
    if (iIndex > 6)
    {
        // Use this index because afBary[] was computed last for it.
        iIndex = 5;
    }

    // fetch Bezier control points
    Real afBez[6] =
    {
        rkTData.Coeff[0],
        rkTData.Coeff[12 + iIndex],
        rkTData.Coeff[13 + (iIndex % 6)],
        rkTData.Coeff[iIndex],
        rkTData.Coeff[6 + iIndex],
        rkTData.Coeff[1 + (iIndex % 6)]
    };

    // evaluate Bezier quadratic
    rfF = afBary[0]*(afBez[0]*afBary[0] + afBez[1]*afBary[1] +
        afBez[2]*afBary[2]) + afBary[1]*(afBez[1]*afBary[0] +
        afBez[3]*afBary[1] + afBez[4]*afBary[2]) + afBary[2]*(
        afBez[2]*afBary[0] + afBez[4]*afBary[1] + afBez[5]*afBary[2]);

    // evaluate barycentric derivatives of F
    Real fFu = ((Real)2.0)*(afBez[0]*afBary[0] + afBez[1]*afBary[1] +
        afBez[2]*afBary[2]);
    Real fFv = ((Real)2.0)*(afBez[1]*afBary[0] + afBez[3]*afBary[1] +
        afBez[4]*afBary[2]);
    Real fFw = ((Real)2.0)*(afBez[2]*afBary[0] + afBez[4]*afBary[1] +
        afBez[5]*afBary[2]);
    Real fDuw = fFu - fFw;
    Real fDvw = fFv - fFw;

    // convert back to (x,y) coordinates
    Real fM00 = kSub0.X() - kSub2.X();
    Real fM10 = kSub0.Y() - kSub2.Y();
    Real fM01 = kSub1.X() - kSub2.X();
    Real fM11 = kSub1.Y() - kSub2.Y();
    Real fInvDet = ((Real)1.0)/(fM00*fM11 - fM10*fM01);

    rfFx = fInvDet*(fM11*fDuw - fM10*fDvw);
    rfFy = fInvDet*(fM00*fDvw - fM01*fDuw);

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntpQdrNonuniform2<float>;

template WM4_FOUNDATION_ITEM
class IntpQdrNonuniform2<double>;
//----------------------------------------------------------------------------
}
