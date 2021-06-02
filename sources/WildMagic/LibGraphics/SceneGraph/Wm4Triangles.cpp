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
// Version: 4.0.3 (2008/01/20)

#include "Wm4GraphicsPCH.h"
#include "Wm4Triangles.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Triangles,Geometry);
WM4_IMPLEMENT_ABSTRACT_STREAM(Triangles);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Triangles,Geometry);

//----------------------------------------------------------------------------
Triangles::Triangles ()
{
    // The Type value will be assigned by the derived class.
}
//----------------------------------------------------------------------------
Triangles::Triangles (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer)
    :
    Geometry(pkVBuffer,pkIBuffer)
{
    // The Type value will be assigned by the derived class.
}
//----------------------------------------------------------------------------
Triangles::~Triangles ()
{
}
//----------------------------------------------------------------------------
bool Triangles::GetModelTriangle (int i, Triangle3f& rkMTri) const
{
    int iV0, iV1, iV2;
    if (GetTriangle(i,iV0,iV1,iV2))
    {
        rkMTri.V[0] = VBuffer->Position3(iV0);
        rkMTri.V[1] = VBuffer->Position3(iV1);
        rkMTri.V[2] = VBuffer->Position3(iV2);
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool Triangles::GetWorldTriangle (int i, Triangle3f& rkWTri) const
{
    int iV0, iV1, iV2;
    if (GetTriangle(i,iV0,iV1,iV2))
    {
        rkWTri.V[0] = World.ApplyForward(VBuffer->Position3(iV0));
        rkWTri.V[1] = World.ApplyForward(VBuffer->Position3(iV1));
        rkWTri.V[2] = World.ApplyForward(VBuffer->Position3(iV2));
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Triangles::GenerateNormals ()
{
    if (!VBuffer->GetAttributes().HasNormal())
    {
        Attributes kAttr = VBuffer->GetAttributes();
        kAttr.SetNormalChannels(3);
        VertexBuffer* pkVBufferPlusNormals = WM4_NEW VertexBuffer(kAttr,
            VBuffer->GetVertexQuantity());
        int iChannels;
        float* afData = pkVBufferPlusNormals->GetData();
        VBuffer->BuildCompatibleArray(kAttr,false,iChannels,afData);
        assert(iChannels == pkVBufferPlusNormals->GetChannelQuantity());
        VBuffer = pkVBufferPlusNormals;
    }

    UpdateModelNormals();
}
//----------------------------------------------------------------------------
bool Triangles::GenerateTangents (int iTextureCoordinateUnit)
{
    Attributes kAttr = VBuffer->GetAttributes();
    if (!kAttr.HasTCoord(iTextureCoordinateUnit))
    {
        return false;
    }

    bool bNeedsModification = false;
    if (!kAttr.HasNormal())
    {
        kAttr.SetNormalChannels(3);
        bNeedsModification = true;
    }
    if (!kAttr.HasTangent())
    {
        kAttr.SetTangentChannels(3);
        bNeedsModification = true;
    }
    if (!kAttr.HasBitangent())
    {
        kAttr.SetBitangentChannels(3);
        bNeedsModification = true;
    }

    if (bNeedsModification)
    {
        VertexBuffer* pkVBufferPlusNTB = WM4_NEW VertexBuffer(kAttr,
            VBuffer->GetVertexQuantity());
        int iChannels;
        float* afData = pkVBufferPlusNTB->GetData();
        VBuffer->BuildCompatibleArray(kAttr,false,iChannels,afData);
        assert(iChannels == pkVBufferPlusNTB->GetChannelQuantity());
        VBuffer = pkVBufferPlusNTB;
    }

    UpdateModelNormals();
    UpdateModelTangentSpace(iTextureCoordinateUnit);
    return true;
}
//----------------------------------------------------------------------------
void Triangles::UpdateModelNormals ()
{
    // Calculate normals from vertices by weighted averages of facet planes
    // that contain the vertices.
    if (!VBuffer->GetAttributes().HasNormal())
    {
        return;
    }

    int iVQuantity = VBuffer->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        VBuffer->Normal3(i) = Vector3f::ZERO;
    }

    int iTQuantity = GetTriangleQuantity();
    for (i = 0; i < iTQuantity; i++)
    {
        // get vertex indices
        int iV0, iV1, iV2;
        if (!GetTriangle(i,iV0,iV1,iV2))
        {
            continue;
        }

        // get vertices
        Vector3f& rkV0 = VBuffer->Position3(iV0);
        Vector3f& rkV1 = VBuffer->Position3(iV1);
        Vector3f& rkV2 = VBuffer->Position3(iV2);

        // compute the normal (length provides the weighted sum)
        Vector3f kEdge1 = rkV1 - rkV0;
        Vector3f kEdge2 = rkV2 - rkV0;
        Vector3f kNormal = kEdge1.Cross(kEdge2);

        VBuffer->Normal3(iV0) += kNormal;
        VBuffer->Normal3(iV1) += kNormal;
        VBuffer->Normal3(iV2) += kNormal;
    }

    for (i = 0; i < iVQuantity; i++)
    {
        VBuffer->Normal3(i).Normalize();
    }
}
//----------------------------------------------------------------------------
void Triangles::UpdateModelTangentSpace (int iTangentSpaceType)
{
    if (iTangentSpaceType <= (int)GU_TANGENT_SPACE_NONE
    ||  !VBuffer->GetAttributes().HasNormal()
    ||  !VBuffer->GetAttributes().HasTangent()
    ||  !VBuffer->GetAttributes().HasBitangent())
    {
        return;
    }

    if (iTangentSpaceType == (int)GU_TANGENT_SPACE_USE_GEOMETRY)
    {
        UpdateTangentSpaceUseGeometry();
    }
    else
    {
        UpdateTangentSpaceUseTCoord(iTangentSpaceType);
    }
}
//----------------------------------------------------------------------------
void Triangles::UpdateTangentSpaceUseGeometry ()
{
    // Compute the matrix of normal derivatives.
    const int iVQuantity = VBuffer->GetVertexQuantity();
    Matrix3f* akDNormal = WM4_NEW Matrix3f[iVQuantity];
    Matrix3f* akWWTrn = WM4_NEW Matrix3f[iVQuantity];
    Matrix3f* akDWTrn = WM4_NEW Matrix3f[iVQuantity];
    memset(akWWTrn,0,iVQuantity*sizeof(Matrix3f));
    memset(akDWTrn,0,iVQuantity*sizeof(Matrix3f));

    const int iTQuantity = GetTriangleQuantity();
    int i, iRow, iCol;
    for (i = 0; i < iTQuantity; i++)
    {
        // Get the triangle vertices.
        int aiV[3];
        if (!GetTriangle(i,aiV[0],aiV[1],aiV[2]))
        {
            continue;
        }

        for (int j = 0; j < 3; j++)
        {
            int iV0 = aiV[j];
            int iV1 = aiV[(j+1)%3];
            int iV2 = aiV[(j+2)%3];

            const Vector3f& rkV0 = VBuffer->Position3(iV0);
            const Vector3f& rkV1 = VBuffer->Position3(iV1);
            const Vector3f& rkV2 = VBuffer->Position3(iV2);
            const Vector3f& rkN0 = VBuffer->Normal3(iV0);
            const Vector3f& rkN1 = VBuffer->Normal3(iV1);
            const Vector3f& rkN2 = VBuffer->Normal3(iV2);

            // Compute edge from V0 to V1, project to tangent plane of vertex,
            // and compute difference of adjacent normals.
            Vector3f kE = rkV1 - rkV0;
            Vector3f kW = kE - kE.Dot(rkN0)*rkN0;
            Vector3f kD = rkN1 - rkN0;
            for (iRow = 0; iRow < 3; iRow++)
            {
                for (iCol = 0; iCol < 3; iCol++)
                {
                    akWWTrn[iV0][iRow][iCol] += kW[iRow]*kW[iCol];
                    akDWTrn[iV0][iRow][iCol] += kD[iRow]*kW[iCol];
                }
            }

            // Compute edge from V0 to V2, project to tangent plane of vertex,
            // and compute difference of adjacent normals.
            kE = rkV2 - rkV0;
            kW = kE - kE.Dot(rkN0)*rkN0;
            kD = rkN2 - rkN0;
            for (iRow = 0; iRow < 3; iRow++)
            {
                for (iCol = 0; iCol < 3; iCol++)
                {
                    akWWTrn[iV0][iRow][iCol] += kW[iRow]*kW[iCol];
                    akDWTrn[iV0][iRow][iCol] += kD[iRow]*kW[iCol];
                }
            }
        }
    }

    // Add in N*N^T to W*W^T for numerical stability.  In theory 0*0^T gets
    // added to D*W^T, but of course no update needed in the implementation.
    // Compute the matrix of normal derivatives.
    for (i = 0; i < iVQuantity; i++)
    {
        const Vector3f& rkNormal = VBuffer->Normal3(i);
        for (iRow = 0; iRow < 3; iRow++)
        {
            for (iCol = 0; iCol < 3; iCol++)
            {
                akWWTrn[i][iRow][iCol] = 0.5f*akWWTrn[i][iRow][iCol] +
                    rkNormal[iRow]*rkNormal[iCol];
                akDWTrn[i][iRow][iCol] *= 0.5f;
            }
        }

        akDNormal[i] = akDWTrn[i]*akWWTrn[i].Inverse();
    }

    WM4_DELETE[] akWWTrn;
    WM4_DELETE[] akDWTrn;

    // If N is a unit-length normal at a vertex, let U and V be unit-length
    // tangents so that {U, V, N} is an orthonormal set.  Define the matrix
    // J = [U | V], a 3-by-2 matrix whose columns are U and V.  Define J^T
    // to be the transpose of J, a 2-by-3 matrix.  Let dN/dX denote the
    // matrix of first-order derivatives of the normal vector field.  The
    // shape matrix is
    //   S = (J^T * J)^{-1} * J^T * dN/dX * J = J^T * dN/dX * J
    // where the superscript of -1 denotes the inverse.  (The formula allows
    // for J built from non-perpendicular vectors.) The matrix S is 2-by-2.
    // The principal curvatures are the eigenvalues of S.  If k is a principal
    // curvature and W is the 2-by-1 eigenvector corresponding to it, then
    // S*W = k*W (by definition).  The corresponding 3-by-1 tangent vector at
    // the vertex is called the principal direction for k, and is J*W.  The
    // principal direction for the minimum principal curvature is stored as
    // the mesh tangent.  The principal direction for the maximum principal
    // curvature is stored as the mesh bitangent.
    for (i = 0; i < iVQuantity; i++)
    {
        // Compute U and V given N.
        const Vector3f& rkNormal = VBuffer->Normal3(i);
        Vector3f kU, kV;
        Vector3f::GenerateComplementBasis(kU,kV,rkNormal);

        // Compute S = J^T * dN/dX * J.  In theory S is symmetric, but
        // because we have estimated dN/dX, we must slightly adjust our
        // calculations to make sure S is symmetric.
        float fS01 = kU.Dot(akDNormal[i]*kV);
        float fS10 = kV.Dot(akDNormal[i]*kU);
        float fSAvr = 0.5f*(fS01+fS10);
        Matrix2f kS
        (
            kU.Dot(akDNormal[i]*kU), fSAvr,
            fSAvr, kV.Dot(akDNormal[i]*kV)
        );

        // Compute the eigenvalues of S (min and max curvatures).
        float fTrace = kS[0][0] + kS[1][1];
        float fDet = kS[0][0]*kS[1][1] - kS[0][1]*kS[1][0];
        float fDiscr = fTrace*fTrace - 4.0f*fDet;
        float fRootDiscr = Mathf::Sqrt(Mathf::FAbs(fDiscr));
        float fMinCurvature = 0.5f*(fTrace - fRootDiscr);
        float fMaxCurvature = 0.5f*(fTrace + fRootDiscr);

        // Compute the eigenvectors of S.
        Vector2f kW0(kS[0][1],fMinCurvature-kS[0][0]);
        Vector2f kW1(fMinCurvature-kS[1][1],kS[1][0]);
        if (kW0.SquaredLength() >= kW1.SquaredLength())
        {
            kW0.Normalize();
            VBuffer->Tangent3(i) = kW0.X()*kU + kW0.Y()*kV;
        }
        else
        {
            kW1.Normalize();
            VBuffer->Tangent3(i) = kW1.X()*kU + kW1.Y()*kV;
        }

        kW0 = Vector2f(kS[0][1],fMaxCurvature-kS[0][0]);
        kW1 = Vector2f(fMaxCurvature-kS[1][1],kS[1][0]);
        if (kW0.SquaredLength() >= kW1.SquaredLength())
        {
            kW0.Normalize();
            VBuffer->Bitangent3(i) = kW0.X()*kU + kW0.Y()*kV;
        }
        else
        {
            kW1.Normalize();
            VBuffer->Bitangent3(i) = kW1.X()*kU + kW1.Y()*kV;
        }
    }

    WM4_DELETE[] akDNormal;
}
//----------------------------------------------------------------------------
void Triangles::UpdateTangentSpaceUseTCoord (int iTextureCoordinateUnit)
{
    // Each vertex can be visited multiple times, so compute the tangent
    // space only on the first visit.  Use the zero vector as a flag for the
    // tangent vector not being computed.
    const int iVQuantity = VBuffer->GetVertexQuantity();
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        VBuffer->Tangent3(i) = Vector3f::ZERO;
    }

    const int iTQuantity = GetTriangleQuantity();
    for (int iT = 0; iT < iTQuantity; iT++)
    {
        // Get the triangle vertices and attributes.
        int iV0, iV1, iV2;
        if (!GetTriangle(iT,iV0,iV1,iV2))
        {
            continue;
        }

        Vector3f* apkPosition[3] =
        {
            &VBuffer->Position3(iV0),
            &VBuffer->Position3(iV1),
            &VBuffer->Position3(iV2)
        };

        Vector3f* apkNormal[3] =
        {
            &VBuffer->Normal3(iV0),
            &VBuffer->Normal3(iV1),
            &VBuffer->Normal3(iV2)
        };

        Vector3f* apkTangent[3] =
        {
            &VBuffer->Tangent3(iV0),
            &VBuffer->Tangent3(iV1),
            &VBuffer->Tangent3(iV2)
        };

        Vector3f* apkBitangent[3] =
        {
            &VBuffer->Bitangent3(iV0),
            &VBuffer->Bitangent3(iV1),
            &VBuffer->Bitangent3(iV2)
        };

        Vector2f* apkTCoord[3] =
        {
            &VBuffer->TCoord2(iTextureCoordinateUnit,iV0),
            &VBuffer->TCoord2(iTextureCoordinateUnit,iV1),
            &VBuffer->TCoord2(iTextureCoordinateUnit,iV2)
        };

        for (i = 0; i < 3; i++)
        {
            if (*apkTangent[i] != Vector3f::ZERO)
            {
                // This vertex has already been visited.
                continue;
            }

            // Compute the tangent space at the vertex.
            Vector3f kNormal = *apkNormal[i];
            int iP = (i == 0) ? 2 : i - 1;
            int iN = (i + 1) % 3;
            Vector3f kTangent = ComputeTangent(
                *apkPosition[i ],*apkTCoord[i ],
                *apkPosition[iN],*apkTCoord[iN],
                *apkPosition[iP],*apkTCoord[iP]);

            // Project T into the tangent plane by projecting out the surface
            // normal N, and then make it unit length.
            kTangent -= kNormal.Dot(kTangent)*kNormal;
            kTangent.Normalize();

            // Compute the bitangent B, another tangent perpendicular to T.
            Vector3f kBitangent = kNormal.UnitCross(kTangent);

            *apkTangent[i] = kTangent;
            *apkBitangent[i] = kBitangent;
        }
    }
}
//----------------------------------------------------------------------------
Vector3f Triangles::ComputeTangent (
    const Vector3f& rkPos0, const Vector2f& rkTCoord0,
    const Vector3f& rkPos1, const Vector2f& rkTCoord1,
    const Vector3f& rkPos2, const Vector2f& rkTCoord2)
{
    // Compute the change in positions at the vertex P0.
    Vector3f kDP1 = rkPos1 - rkPos0;
    Vector3f kDP2 = rkPos2 - rkPos0;

    if (Mathf::FAbs(kDP1.Length()) < Mathf::ZERO_TOLERANCE
    ||  Mathf::FAbs(kDP2.Length()) < Mathf::ZERO_TOLERANCE)
    {
        // The triangle is very small, call it degenerate.
        return Vector3f::ZERO;
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
            return Vector3f::ZERO;
        }

        // The variation is effectively all in u, so set the tangent vector
        // to be T = dP/du.
        return kDP1/fDU1;
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
        return Vector3f::ZERO;
    }

    // The triangle vertices are not collinear in parameter space, so choose
    // the tangent to be dP/du = (dv1*dP2-dv2*dP1)/(dv1*du2-dv2*du1)
    return (fDV1*kDP2-fDV2*kDP1)/fDet;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Triangles::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;
    Geometry::Load(rkStream,pkLink);
    WM4_END_DEBUG_STREAM_LOAD(Triangles);
}
//----------------------------------------------------------------------------
void Triangles::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Geometry::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool Triangles::Register (Stream& rkStream) const
{
    return Geometry::Register(rkStream);
}
//----------------------------------------------------------------------------
void Triangles::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;
    Geometry::Save(rkStream);
    WM4_END_DEBUG_STREAM_SAVE(Triangles);
}
//----------------------------------------------------------------------------
int Triangles::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Geometry::GetDiskUsed(rkVersion);
}
//----------------------------------------------------------------------------
StringTree* Triangles::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Geometry::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
