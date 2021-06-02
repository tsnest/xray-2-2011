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
// Version: 4.0.3 (2007/08/11)

#include "SimpleBumpMapEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,SimpleBumpMapEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(SimpleBumpMapEffect);
WM4_IMPLEMENT_DEFAULT_NAME_ID(SimpleBumpMapEffect,ShaderEffect);

//----------------------------------------------------------------------------
SimpleBumpMapEffect::SimpleBumpMapEffect (const char* acBaseName,
    const char* acNormalName, const Vector3f& rkLightDirection)
    :
    ShaderEffect(1),
    m_kLightDirection(rkLightDirection)
{
    m_kVShader[0] = WM4_NEW VertexShader("SimpleBumpMap");
    m_kPShader[0] = WM4_NEW PixelShader("SimpleBumpMap");

    m_kPShader[0]->SetTexture(0,acBaseName);
    m_kPShader[0]->SetTexture(1,acNormalName);
}
//----------------------------------------------------------------------------
SimpleBumpMapEffect::SimpleBumpMapEffect ()
{
}
//----------------------------------------------------------------------------
SimpleBumpMapEffect::~SimpleBumpMapEffect ()
{
}
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::SetLightDirection (const Vector3f& rkLightDirection)
{
    m_kLightDirection = rkLightDirection;
}
//----------------------------------------------------------------------------
const Vector3f& SimpleBumpMapEffect::GetLightDirection () const
{
    return m_kLightDirection;
}
//----------------------------------------------------------------------------
bool SimpleBumpMapEffect::ComputeTangent (const Vector3f& rkPos0,
    const Vector2f& rkTCoord0, const Vector3f& rkPos1,
    const Vector2f& rkTCoord1, const Vector3f& rkPos2,
    const Vector2f& rkTCoord2, Vector3f& rkTangent)
{
    // Compute the change in positions at the vertex P0.
    Vector3f kDP1 = rkPos1 - rkPos0;
    Vector3f kDP2 = rkPos2 - rkPos0;

    if (Mathf::FAbs(kDP1.Length()) < Mathf::ZERO_TOLERANCE
    ||  Mathf::FAbs(kDP2.Length()) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle is very small, call it degenerate.
        return false;
    }

    // Compute the change in texture coordinates at the vertex P0 in the
    // direction of edge P1-P0.
    float fDU1 = rkTCoord1[0] - rkTCoord0[0];
    float fDV1 = rkTCoord1[1] - rkTCoord0[1];
    if (Mathf::FAbs(fDV1) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle effectively has no variation in the v texture
        // coordinate.
        if (Mathf::FAbs(fDU1) < Mathf::ZERO_TOLERANCE)
        {
            // The triangle effectively has no variation in the u coordinate.
            // Since the texture coordinates do not vary on this triangle,
            // treat it as a degenerate parametric surface.
            return false;
        }

        // The variation is effectively all in u, so set the tangent vector
        // to be T = dP/du.
        rkTangent = kDP1/fDU1;
        return true;
    }

    // Compute the change in texture coordinates at the vertex P0 in the
    // direction of edge P2-P0.
    float fDU2 = rkTCoord2[0] - rkTCoord0[0];
    float fDV2 = rkTCoord2[1] - rkTCoord0[1];
    float fDet = fDV1*fDU2 - fDV2*fDU1;
    if (Mathf::FAbs(fDet) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle vertices are collinear in parameter space, so treat
        // this as a degenerate parametric surface.
        return false;
    }

    // The triangle vertices are not collinear in parameter space, so choose
    // the tangent to be dP/du = (dv1*dP2-dv2*dP1)/(dv1*du2-dv2*du1)
    rkTangent = (fDV1*kDP2-fDV2*kDP1)/fDet;
    return true;
}
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::ComputeLightVectors (Triangles* pkMesh)
{
    // The tangent-space coordinates for the light direction vector at each
    // vertex is stored in the color0 channel.  The computations use the
    // vertex normals and the texture coordinates for the base mesh, which
    // are stored in the tcoord0 channel.
    assert(pkMesh && pkMesh->VBuffer && pkMesh->IBuffer);
    assert(pkMesh->VBuffer->GetAttributes().GetPositionChannels() == 3);
    assert(pkMesh->VBuffer->GetAttributes().GetNormalChannels() == 3);
    assert(pkMesh->VBuffer->GetAttributes().GetColorChannels(0) == 3);
    assert(pkMesh->VBuffer->GetAttributes().GetTCoordChannels(0) == 2);

    // The light direction D is in world-space coordinates.  Negate it,
    // transform it to model-space coordinates, and then normalize it.  The
    // world-space direction is unit-length, but the geometric primitive
    // might have non-unit scaling in its model-to-world transformation, in
    // which case the normalization is necessary.
    Vector3f kModelLightDirection = -m_kLightDirection;
    kModelLightDirection = pkMesh->World.InvertVector(kModelLightDirection);

    // Set the light vectors to (0,0,0) as a flag that the quantity has not
    // yet been computed.  The probability that a light vector is actually
    // (0,0,0) should be small, so the flag system should save computation
    // time overall.
    VertexBuffer* pkVB = pkMesh->VBuffer;
    int iVQuantity = pkMesh->VBuffer->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        pkVB->Color3(0,i) = ColorRGB::BLACK;
    }

    int iTQuantity = pkMesh->GetTriangleQuantity();
    for (int iT = 0; iT < iTQuantity; iT++)
    {
        // Get the triangle vertices and attributes.
        int iV0, iV1, iV2;
        if (!pkMesh->GetTriangle(iT,iV0,iV1,iV2))
        {
            continue;
        }

        Vector3f* apkV[3] =
        {
            &pkVB->Position3(iV0),
            &pkVB->Position3(iV1),
            &pkVB->Position3(iV2)
        };

        Vector3f* apkN[3] =
        {
            &pkVB->Normal3(iV0),
            &pkVB->Normal3(iV1),
            &pkVB->Normal3(iV2)
        };

        ColorRGB* apkC[3] =
        {
            &pkVB->Color3(0,iV0),
            &pkVB->Color3(0,iV1),
            &pkVB->Color3(0,iV2)
        };

        Vector2f* apkST[3] =
        {
            &pkVB->TCoord2(0,iV0),
            &pkVB->TCoord2(0,iV1),
            &pkVB->TCoord2(0,iV2)
        };

        for (i = 0; i < 3; i++)
        {
            ColorRGB& rkColor = *apkC[i];
            if (rkColor != ColorRGB::BLACK)
            {
                continue;
            }

            int iP = (i == 0) ? 2 : i - 1;
            int iN = (i + 1) % 3;

            Vector3f kTangent;
            if (!ComputeTangent(*apkV[i],*apkST[i],*apkV[iN],*apkST[iN],
                *apkV[iP],*apkST[iP],kTangent))
            {
                // The texture coordinate mapping is not properly defined for
                // this.  Just say that the tangent space light vector points
                // in the same direction as the surface normal.
                rkColor.R() = apkN[i]->X();
                rkColor.G() = apkN[i]->Y();
                rkColor.B() = apkN[i]->Z();
                continue;
            }

            // Project T into the tangent plane by projecting out the surface
            // normal N, and then make it unit length.
            kTangent -= apkN[i]->Dot(kTangent)*(*apkN[i]);
            kTangent.Normalize();

            // Compute the bitangent B, another tangent perpendicular to T.
            Vector3f kBitangent = apkN[i]->UnitCross(kTangent);

            // The set {T,B,N} is a right-handed orthonormal set.  The
            // negated light direction U = -D is represented in this
            // coordinate system as
            //   U = Dot(U,T)*T + Dot(U,B)*B + Dot(U,N)*N
            float fDotUT = kModelLightDirection.Dot(kTangent);
            float fDotUB = kModelLightDirection.Dot(kBitangent);
            float fDotUN = kModelLightDirection.Dot(*apkN[i]);

            // Transform the light vector into [0,1]^3 to make it a valid
            // ColorRGB object.
            rkColor.R() = 0.5f*(fDotUT + 1.0f);
            rkColor.G() = 0.5f*(fDotUB + 1.0f);
            rkColor.B() = 0.5f*(fDotUN + 1.0f);
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_kLightDirection);

    WM4_END_DEBUG_STREAM_LOAD(SimpleBumpMapEffect);
}
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool SimpleBumpMapEffect::Register (Stream& rkStream) const
{
    return ShaderEffect::Register(rkStream);
}
//----------------------------------------------------------------------------
void SimpleBumpMapEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // native data
    rkStream.Write(m_kLightDirection);

    WM4_END_DEBUG_STREAM_SAVE(SimpleBumpMapEffect);
}
//----------------------------------------------------------------------------
int SimpleBumpMapEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion) +
        sizeof(m_kLightDirection);
}
//----------------------------------------------------------------------------
StringTree* SimpleBumpMapEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("light direction =",m_kLightDirection));

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
