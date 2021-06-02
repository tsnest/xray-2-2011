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
#include "Wm4SurfacePatch.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SurfacePatch,Object);
WM4_IMPLEMENT_ABSTRACT_STREAM(SurfacePatch);
WM4_IMPLEMENT_DEFAULT_NAME_ID(SurfacePatch,Object);

//----------------------------------------------------------------------------
SurfacePatch::SurfacePatch (float fUMin, float fUMax, float fVMin,
    float fVMax, bool bRectangular)
{
    assert(fUMin < fUMax && fVMin < fVMax);

    m_fUMin = fUMin;
    m_fUMax = fUMax;
    m_fVMin = fVMin;
    m_fVMax = fVMax;
    m_bRectangular = bRectangular;
}
//----------------------------------------------------------------------------
SurfacePatch::SurfacePatch ()
{
    m_fUMin = 0.0f;
    m_fUMax = 0.0f;
    m_fVMin = 0.0f;
    m_fVMax = 0.0f;
    m_bRectangular = false;
}
//----------------------------------------------------------------------------
SurfacePatch::~SurfacePatch ()
{
}
//----------------------------------------------------------------------------
void SurfacePatch::GetFrame (float fU, float fV, Vector3f& rkPosition,
    Vector3f& rkTangent0, Vector3f& rkTangent1, Vector3f& rkNormal) const
{
    rkPosition = P(fU,fV);

    rkTangent0 = PU(fU,fV);
    rkTangent1 = PV(fU,fV);
    rkTangent0.Normalize();  // T0
    rkTangent1.Normalize();  // temporary T1 just to compute N
    rkNormal = rkTangent0.UnitCross(rkTangent1);  // N

    // The normalized first derivatives are not necessarily orthogonal.
    // Recompute T1 so that {T0,T1,N} is an orthonormal set.
    rkTangent1 = rkNormal.Cross(rkTangent0);
}
//----------------------------------------------------------------------------
void SurfacePatch::ComputePrincipalCurvatureInfo (float fU, float fV,
    float& rfCurv0, float& rfCurv1, Vector3f& rkDir0, Vector3f& rkDir1)
{
    // Tangents:  T0 = (x_u,y_u,z_u), T1 = (x_v,y_v,z_v)
    // Normal:    N = Cross(T0,T1)/Length(Cross(T0,T1))
    // Metric Tensor:    G = +-                      -+
    //                       | Dot(T0,T0)  Dot(T0,T1) |
    //                       | Dot(T1,T0)  Dot(T1,T1) |
    //                       +-                      -+
    //
    // Curvature Tensor:  B = +-                          -+
    //                        | -Dot(N,T0_u)  -Dot(N,T0_v) |
    //                        | -Dot(N,T1_u)  -Dot(N,T1_v) |
    //                        +-                          -+
    //
    // Principal curvatures k are the generalized eigenvalues of
    //
    //     Bw = kGw
    //
    // If k is a curvature and w=(a,b) is the corresponding solution to
    // Bw = kGw, then the principal direction as a 3D vector is d = a*U+b*V.
    //
    // Let k1 and k2 be the principal curvatures.  The mean curvature
    // is (k1+k2)/2 and the Gaussian curvature is k1*k2.

    // derivatives
    Vector3f kDerU = PU(fU,fV);
    Vector3f kDerV = PV(fU,fV);
    Vector3f kDerUU = PUU(fU,fV);
    Vector3f kDerUV = PUV(fU,fV);
    Vector3f kDerVV = PVV(fU,fV);

    // metric tensor
    Matrix2f kMetricTensor;
    kMetricTensor[0][0] = kDerU.Dot(kDerU);
    kMetricTensor[0][1] = kDerU.Dot(kDerV);
    kMetricTensor[1][0] = kMetricTensor[1][0];
    kMetricTensor[1][1] = kDerV.Dot(kDerV);

    // curvature tensor
    Vector3f kNormal = kDerU.UnitCross(kDerV);
    Matrix2f kCurvatureTensor;
    kCurvatureTensor[0][0] = -kNormal.Dot(kDerUU);
    kCurvatureTensor[0][1] = -kNormal.Dot(kDerUV);
    kCurvatureTensor[1][0] = kCurvatureTensor[0][1];
    kCurvatureTensor[1][1] = -kNormal.Dot(kDerVV);

    // characteristic polynomial is 0 = det(B-kG) = c2*k^2+c1*k+c0
    float fC0 = kCurvatureTensor.Determinant();
    float fC1 = 2.0f*kCurvatureTensor[0][1]* kMetricTensor[0][1] -
        kCurvatureTensor[0][0]*kMetricTensor[1][1] -
        kCurvatureTensor[1][1]*kMetricTensor[0][0];
    float fC2 = kMetricTensor.Determinant();

    // principal curvatures are roots of characteristic polynomial
    float fTemp = Mathf::Sqrt(Mathf::FAbs(fC1*fC1 -4.0f*fC0*fC2));
    rfCurv0 = -0.5f*(fC1+fTemp);
    rfCurv1 = 0.5f*(-fC1+fTemp);

    // principal directions are solutions to (B-kG)w = 0
    // w1 = (b12-k1*g12,-(b11-k1*g11)) OR (b22-k1*g22,-(b12-k1*g12))
    float fA0 = kCurvatureTensor[0][1] - rfCurv0*kMetricTensor[0][1];
    float fA1 = rfCurv0*kMetricTensor[0][0] - kCurvatureTensor[0][0];
    float fLength = Mathf::Sqrt(fA0*fA0+fA1*fA1);
    if (fLength >= Mathf::ZERO_TOLERANCE)
    {
        rkDir0 = fA0*kDerU + fA1*kDerV;
    }
    else
    {
        fA0 = kCurvatureTensor[1][1] - rfCurv0*kMetricTensor[1][1];
        fA1 = rfCurv0*kMetricTensor[0][1] - kCurvatureTensor[0][1];
        fLength = Mathf::Sqrt(fA0*fA0+fA1*fA1);
        if (fLength >= Mathf::ZERO_TOLERANCE)
        {
            rkDir0 = fA0*kDerU + fA1*kDerV;
        }
        else
        {
            // umbilic (surface is locally sphere, any direction principal)
            rkDir0 = kDerU;
        }
    }
    rkDir0.Normalize();

    // second tangent is cross product of first tangent and normal
    rkDir1 = rkDir0.Cross(kNormal);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SurfacePatch::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Object::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_fUMin);
    rkStream.Read(m_fUMax);
    rkStream.Read(m_fVMin);
    rkStream.Read(m_fVMax);
    rkStream.Read(m_bRectangular);

    WM4_END_DEBUG_STREAM_LOAD(SurfacePatch);
}
//----------------------------------------------------------------------------
void SurfacePatch::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Object::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SurfacePatch::Register (Stream& rkStream) const
{
    return Object::Register(rkStream);
}
//----------------------------------------------------------------------------
void SurfacePatch::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Object::Save(rkStream);

    // native data
    rkStream.Write(m_fUMin);
    rkStream.Write(m_fUMax);
    rkStream.Write(m_fVMin);
    rkStream.Write(m_fVMax);
    rkStream.Write(m_bRectangular);

    WM4_END_DEBUG_STREAM_SAVE(SurfacePatch);
}
//----------------------------------------------------------------------------
int SurfacePatch::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Object::GetDiskUsed(rkVersion) +
        sizeof(m_fUMin) +
        sizeof(m_fUMax) +
        sizeof(m_fVMin) +
        sizeof(m_fVMax) +
        sizeof(char);  // m_bRectangular
}
//----------------------------------------------------------------------------
StringTree* SurfacePatch::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("umin =",m_fUMin));
    pkTree->Append(Format("umax =",m_fUMax));
    pkTree->Append(Format("vmin =",m_fVMin));
    pkTree->Append(Format("vmax =",m_fVMax));
    pkTree->Append(Format("rect =",m_bRectangular));

    // children
    pkTree->Append(Object::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
